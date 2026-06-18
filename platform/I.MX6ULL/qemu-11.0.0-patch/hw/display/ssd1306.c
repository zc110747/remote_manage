/*
 * SSD1306 I2C OLED Emulator
 *
 * Compatible:
 *      Linux drivers/video/fbdev/ssd1307fb.c
 *
 * Address:
 *      0x3c
 *
 * Output:
 *      /tmp/oled.pgm
 */

#include "qemu/osdep.h"
#include "hw/i2c/i2c.h"
#include "migration/vmstate.h"
#include "qapi/error.h"
#include "qemu/module.h"

#define TYPE_SSD1306 "ssd1306"

OBJECT_DECLARE_SIMPLE_TYPE(SSD1306State, SSD1306)

#define OLED_WIDTH     128
#define OLED_HEIGHT     64

#define OLED_GDDRAM_SIZE \
    (OLED_WIDTH * OLED_HEIGHT / 8)

typedef struct SSD1306State {

    I2CSlave parent_obj;

    bool waiting_control;

    uint8_t control;

    uint8_t gddram[OLED_GDDRAM_SIZE];

    uint8_t page;
    uint8_t column;

    uint8_t page_start;
    uint8_t page_end;

    uint8_t col_start;
    uint8_t col_end;

    bool display_on;

    bool cmd_pending;
    uint8_t cmd;
    uint8_t cmd_idx;

} SSD1306State;

/* ------------------------------------------------ */
/* PGM Export                                       */
/* ------------------------------------------------ */

static void ssd1306_dump_pgm(SSD1306State *s)
{
    FILE *fp;
    int x,y;

    fp = fopen("/tmp/oled.pgm","wb");
    if (!fp) {
        return;
    }

    fprintf(fp,
            "P5\n"
            "%d %d\n"
            "255\n",
            OLED_WIDTH,
            OLED_HEIGHT);

    for (y = 0; y < OLED_HEIGHT; y++) {

        for (x = 0; x < OLED_WIDTH; x++) {

            int page = y / 8;
            int bit  = y & 7;

            int idx =
                page * OLED_WIDTH + x;

            uint8_t pixel =
                (s->gddram[idx] &
                 (1 << bit))
                ? 255 : 0;

            fwrite(&pixel,1,1,fp);
        }
    }

    fclose(fp);
}

/* ------------------------------------------------ */
/* GDDRAM                                           */
/* ------------------------------------------------ */

static void ssd1306_write_data(
        SSD1306State *s,
        uint8_t val)
{
    int idx;

    idx =
        s->page * OLED_WIDTH +
        s->column;

    if (idx < OLED_GDDRAM_SIZE) {
        s->gddram[idx] = val;
    }

    s->column++;

    if (s->column > s->col_end) {

        s->column = s->col_start;

        s->page++;

        if (s->page > s->page_end) {
            s->page = s->page_start;

            if (s->display_on) {
                ssd1306_dump_pgm(s);
            }
        }
    }
}

/* ------------------------------------------------ */
/* Commands                                         */
/* ------------------------------------------------ */

static void ssd1306_command(
        SSD1306State *s,
        uint8_t value)
{
    if (s->cmd_pending) {

        switch (s->cmd) {

        case 0x21:

            if (s->cmd_idx == 0) {
                s->col_start = value;
                s->column = value;
                s->cmd_idx++;
                return;
            }

            s->col_end = value;
            s->cmd_pending = false;
            return;

        case 0x22:

            if (s->cmd_idx == 0) {
                s->page_start = value;
                s->page = value;
                s->cmd_idx++;
                return;
            }

            s->page_end = value;
            s->cmd_pending = false;
            return;

        case 0x81:
        case 0xA8:
        case 0xD3:
        case 0xD5:
        case 0xD9:
        case 0xDB:
        case 0x8D:

            s->cmd_pending = false;
            return;

        default:
            s->cmd_pending = false;
            return;
        }
    }

    switch (value) {

    case 0xAE:
        s->display_on = false;
        return;

    case 0xAF:
        s->display_on = true;
        ssd1306_dump_pgm(s);
        return;

    case 0x21:
    case 0x22:

        s->cmd = value;
        s->cmd_idx = 0;
        s->cmd_pending = true;
        return;

    case 0x81:
    case 0xA8:
    case 0xD3:
    case 0xD5:
    case 0xD9:
    case 0xDB:
    case 0x8D:

        s->cmd = value;
        s->cmd_pending = true;
        s->cmd_idx = 0;
        return;

    default:
        return;
    }
}

/* ------------------------------------------------ */
/* I2C                                              */
/* ------------------------------------------------ */

static int ssd1306_send(
        I2CSlave *i2c,
        uint8_t data)
{
    SSD1306State *s = SSD1306(i2c);

    if (s->waiting_control) {

        s->control = data;
        s->waiting_control = false;

        return 0;
    }

    switch (s->control) {

    case 0x80:
        ssd1306_command(s, data);
        break;

    case 0x40:
        ssd1306_write_data(s, data);
        break;

    default:
        break;
    }

    return 0;
}

static uint8_t ssd1306_recv(
        I2CSlave *i2c)
{
    return 0xff;
}

static int ssd1306_event(
        I2CSlave *i2c,
        enum i2c_event event)
{
    SSD1306State *s = SSD1306(i2c);

    switch(event) {

    case I2C_START_SEND:
        s->waiting_control = true;
        break;

    case I2C_START_RECV:
        break;

    case I2C_FINISH:
        break;

    case I2C_NACK:
        break;

    default:
        break;
    }

    return 0;
}

/* ------------------------------------------------ */
/* Reset                                            */
/* ------------------------------------------------ */

static void ssd1306_reset_state(
        SSD1306State *s)
{
    memset(s->gddram,0,sizeof(s->gddram));

    s->waiting_control = true;

    s->page = 0;
    s->column = 0;

    s->page_start = 0;
    s->page_end = 7;

    s->col_start = 0;
    s->col_end = 127;

    s->display_on = false;

    s->cmd_pending = false;
}

static void ssd1306_reset_hold(
        Object *obj,
        ResetType type)
{
    SSD1306State *s =
        SSD1306(obj);

    ssd1306_reset_state(s);
}

/* ------------------------------------------------ */
/* Realize                                          */
/* ------------------------------------------------ */

static void ssd1306_realize(
        DeviceState *dev,
        Error **errp)
{
    SSD1306State *s =
        SSD1306(dev);

    ssd1306_reset_state(s);
}

/* ------------------------------------------------ */
/* VMSTATE                                          */
/* ------------------------------------------------ */

static const VMStateDescription
vmstate_ssd1306 = {

    .name = "ssd1306",
    .version_id = 1,
    .minimum_version_id = 1,

    .fields = (const VMStateField[]) {

        VMSTATE_BOOL(waiting_control,
                     SSD1306State),

        VMSTATE_UINT8(control,
                      SSD1306State),

        VMSTATE_UINT8_ARRAY(
            gddram,
            SSD1306State,
            OLED_GDDRAM_SIZE),

        VMSTATE_UINT8(page,
                      SSD1306State),

        VMSTATE_UINT8(column,
                      SSD1306State),

        VMSTATE_BOOL(display_on,
                     SSD1306State),

        VMSTATE_END_OF_LIST()
    }
};

/* ------------------------------------------------ */
/* QOM                                              */
/* ------------------------------------------------ */

static void ssd1306_init(
        Object *obj)
{
    SSD1306State *s =
        SSD1306(obj);

    s->waiting_control = true;
}

static void ssd1306_class_init(
        ObjectClass *klass,
        const void *data)
{
    DeviceClass *dc =
        DEVICE_CLASS(klass);

    I2CSlaveClass *k =
        I2C_SLAVE_CLASS(klass);

    ResettableClass *rc =
        RESETTABLE_CLASS(klass);

    dc->realize = ssd1306_realize;
    dc->vmsd = &vmstate_ssd1306;

    rc->phases.hold =
        ssd1306_reset_hold;

    k->send  = ssd1306_send;
    k->recv  = ssd1306_recv;
    k->event = ssd1306_event;
}

static const TypeInfo
ssd1306_type_info = {

    .name = TYPE_SSD1306,
    .parent = TYPE_I2C_SLAVE,

    .instance_size =
        sizeof(SSD1306State),

    .instance_init =
        ssd1306_init,

    .class_init =
        ssd1306_class_init,
};

static void
ssd1306_register_types(void)
{
    type_register_static(
        &ssd1306_type_info);
}

type_init(ssd1306_register_types)
