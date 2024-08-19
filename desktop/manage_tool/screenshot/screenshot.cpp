/*!
    屏幕截图的相关应用实现
*/

#include "screenshot.h"
#include <QScreen>

static CScreenShot *pLocalCScreenShotInfo;

CScreenShot::CScreenShot()
{
    setWindowState(Qt::WindowActive|Qt::WindowFullScreen);
    tipWidth = 300; //温馨提示框的宽度
    tipHeight = 100; //温馨提示框的高度
    infoWidth = 150; //坐标信息框的宽度
    infoHeight = 50; //坐标信息框的高度
    initCScreenShot();
}

void CScreenShot::initSelectedMenu()
{
    savePixmapAction = new QAction(tr("保存选择区域"),this);
    cancelAction = new QAction(tr("重选"),this);
    quitAction = new QAction(tr("退出"),this);
    contextMenu = new QMenu(this);

    connect(savePixmapAction, SIGNAL(triggered()),this, SLOT(savePixmap()));
    connect(cancelAction, SIGNAL(triggered()),this, SLOT(cancelSelectedRect()));
    connect(quitAction, SIGNAL(triggered()),this, SLOT(hide()));
}

void CScreenShot::Release()
{
    if (isMainWinHide)
    {
        emit send_release();
    }
    hide();
    close();
}

void CScreenShot::savePixmap()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,tr("保存图片"),QDir::currentPath(),tr("Images (*.jpg *.png *.bmp)"));
    if (fileName.isNull())
        return;

    shotPixmap.save(fileName);
    Release();
}

void CScreenShot::loadBackgroundPixmap(const QPixmap &bgPixmap)
{
    int width, height;
    QScreen *mScreen = QGuiApplication::screens().first();

    width = mScreen->geometry().width();
    height = mScreen->geometry().height();
    loadBackgroundPixmap(bgPixmap, 0, 0, width,height);
}

void CScreenShot::loadBackgroundPixmap(const QPixmap &bgPixmap, int x, int y, int width, int height)
{
    loadPixmap = bgPixmap;
    screenx = x;
    screeny = y;
    screenwidth = width;
    screenheight = height;
    initCScreenShot();
}

QPixmap CScreenShot::getFullScreenPixmap()
{
    initCScreenShot();

    QPixmap QImgMap;
    QScreen *mScreen = QGuiApplication::screens().first();
    QImgMap = mScreen->grabWindow(0);

    return QImgMap;
}

void CScreenShot::paintEvent(QPaintEvent *event)
{
    QColor shadowColor;
    shadowColor= QColor(0,0,0,100); //阴影颜色设置
    painter.begin(this); //进行重绘

    painter.setPen(QPen(Qt::blue,2,Qt::SolidLine,Qt::FlatCap));//设置画笔
    painter.drawPixmap(screenx,screeny,loadPixmap); //将背景图片画到窗体上
    painter.fillRect(screenx,screeny,screenwidth,screenheight,shadowColor); //画影罩效果

    switch (currentShotState)
    {
        case initShot:
          drawTipsText();
          break;
        case beginShot:
        case finishShot:
          selectedRect = getRect(beginPoint,endPoint); //获取选区
          drawSelectedPixmap();
          break;
        case beginMoveShot:
        case finishMoveShot:
          selectedRect = getMoveAllSelectedRect(); //获取选区
          drawSelectedPixmap();
          break;
        case beginControl:
        case finishControl:
          selectedRect = getMoveControlSelectedRect();
          drawSelectedPixmap();
          break;
        default:
          break;
    }
    drawXYWHInfo(); //打印坐标信息
    painter.end();  //重绘结束

    if (currentShotState == finishMoveShot || currentShotState == finishControl)
    {
        updateBeginEndPointValue(selectedRect); //当移动完选区后，更新beginPoint,endPoint;为下一次移动做准备工作
    }

    qDebug()<<event->rect();
}

void CScreenShot::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        initCScreenShot();
        //hide();
        Release();
    }
}

void CScreenShot::mousePressEvent(QMouseEvent *event)
{
    //当开始进行拖动进行选择区域时,确定开始选取的beginPoint坐标
    if (event->button() == Qt::LeftButton && currentShotState == initShot)
    {
        currentShotState = beginShot; //设置当前状态为beginShot状态
        beginPoint = event->pos();
    }

    //移动选区改变选区的所在位置
    if (event->button() == Qt::LeftButton && isInSelectedRect(event->pos()) &&
    getMoveControlState(event->pos()) == moveControl0)
    {
        currentShotState = beginMoveShot; //启用开始移动选取选项,beginMoveShot状态
        moveBeginPoint = event->pos();
    }

    //移动控制点改变选区大小
    if (event->button() == Qt::LeftButton && getMoveControlState(event->pos()) != moveControl0
    ){
        currentShotState = beginControl; //开始移动控制点
        controlValue = getMoveControlState(event->pos());
        moveBeginPoint = event->pos();
    }
}

void CScreenShot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && currentShotState == beginShot)
    {
        currentShotState = finishShot;
        endPoint = event->pos();
        update();
    }

    if (event->button() == Qt::LeftButton && currentShotState == beginMoveShot)
    {
        currentShotState = finishMoveShot;
        moveEndPoint = event->pos();
        update();
    }

    //当前状态为beginControl状态时，设置状态为finishControl
    if (event->button() == Qt::LeftButton && currentShotState == beginControl)
    {
        currentShotState = finishControl;
        moveEndPoint = event->pos();
        update();
    }
}

void CScreenShot::mouseMoveEvent(QMouseEvent *event)
{
    //当拖动时，动态的更新所选择的区域
    if (currentShotState == beginShot){
        endPoint = event->pos();
        update();
    }

    //当确定选区后，对选区进行移动操作
    if (currentShotState == beginMoveShot || currentShotState == beginControl)
    {
        moveEndPoint = event->pos();
        update();
    }

    updateMouseShape(event->pos()); //修改鼠标的形状
    setMouseTracking(true);
}

void CScreenShot::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (currentShotState == finishShot || currentShotState == finishMoveShot
    || currentShotState == finishControl)
    {
        Release();
    }
    qDebug()<<event->pos();
}

QRect CScreenShot::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
    int x,y,width,height;
    width = qAbs(beginPoint.x() - endPoint.x());
    height = qAbs(beginPoint.y() - endPoint.y());
    x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
    y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

    return QRect(x,y,width,height);
}

void CScreenShot::initCScreenShot()
{
    currentShotState = initShot;
    controlValue = moveControl0;
    beginPoint =QPoint(0,0);
    endPoint = QPoint(0,0);
    moveBeginPoint = QPoint(0,0);
    moveEndPoint = QPoint(0,0);

    tlRect = QRect(0,0,0,0); //左上点
    trRect = QRect(0,0,0,0); //上右点
    blRect = QRect(0,0,0,0); //左下点
    brRect = QRect(0,0,0,0); //右下点
    tcRect = QRect(0,0,0,0); //上中点
    bcRect = QRect(0,0,0,0); //下中点
    lcRect = QRect(0,0,0,0); //左中点
    rcRect = QRect(0,0,0,0); //右中点

    setCursor(Qt::CrossCursor);
}

bool CScreenShot::isInSelectedRect(const QPoint &point)
{
    int x,y;
    QRect selectedRect;
    if (currentShotState == initShot || currentShotState == beginShot)
    {
        return false;
    }


    selectedRect = getSelectedRect();
    x = point.x();
    y = point.y();

    return selectedRect.contains(x,y);
}

void CScreenShot::cancelSelectedRect()
{
    initCScreenShot();
    update(); //进行重绘，将选取区域去掉
}

void CScreenShot::contextMenuEvent(QContextMenuEvent *event)
{
    initSelectedMenu();

    if (isInSelectedRect(event->pos()))
    {
        contextMenu->addAction(savePixmapAction);
    }
    else
    {
        contextMenu->addAction(cancelAction);
        contextMenu->addAction(quitAction);
    }

    contextMenu->exec(event->pos());
}

void CScreenShot::drawTipsText()
{
    int x = (screenwidth - tipWidth)/2;
    int y = (screenheight - tipHeight)/2;
    QColor color = QColor(100,100,100,200);
    QRect rect = QRect(x,y,tipWidth,tipHeight);
    QString strTipsText = QString(tr("温馨提示\n鼠标拖动进行截屏;截屏区域内右键保存;\n截屏区域外右键取消;ESC退出;"));

    painter.fillRect(rect,color);
    painter.setPen(QPen(Qt::white));//设置画笔的颜色为白色
    painter.drawText(rect,Qt::AlignCenter,strTipsText);
}

QRect CScreenShot::getSelectedRect()
{
    if (currentShotState == beginMoveShot)
    {
        return getMoveAllSelectedRect();
    }
    else if (currentShotState == beginControl)
    {
        return getMoveControlSelectedRect();
    }
    else
    {
        return getRect(beginPoint,endPoint);
    }
}

void CScreenShot::updateBeginEndPointValue(const QRect &rect)
{
    beginPoint = rect.topLeft();
    endPoint = rect.bottomRight();

    moveBeginPoint = QPoint(0,0);
    moveEndPoint = QPoint(0,0);
}

void CScreenShot::checkMoveEndPoint()
{
    int x,y;
    QRect selectedRect = getRect(beginPoint, endPoint);
    QPoint bottomRightPoint = selectedRect.bottomRight();
    x = moveEndPoint.x() - moveBeginPoint.x();
    y = moveEndPoint.y() - moveBeginPoint.y();

    //当移动后X坐标小于零时，则出现选区丢失，则计算出moveEndPoint的X最大坐标值，进行赋值
    if (x + selectedRect.x() < 0)
    {
        moveEndPoint.setX(qAbs(selectedRect.x()-moveBeginPoint.x()));
    }

    if (y + selectedRect.y() < 0)
    {
        moveEndPoint.setY(qAbs(selectedRect.y() - moveBeginPoint.y()));
    }

    if (x + bottomRightPoint.x() > screenwidth)
    {
        moveEndPoint.setX(screenwidth - bottomRightPoint.x() + moveBeginPoint.x());
    }

    if (y + bottomRightPoint.y() > screenheight)
    {
        moveEndPoint.setY(screenheight - bottomRightPoint.y() + moveBeginPoint.y());
    }
}

void CScreenShot::draw8ControlPoint(const QRect &rect)
{
    int x,y;
    QColor color= QColor(0,0,255); //画点的颜色设置
    QPoint tlPoint = rect.topLeft(); //左上点
    QPoint trPoint = rect.topRight(); //右上点
    QPoint blPoint = rect.bottomLeft(); //左下点
    QPoint brPoint = rect.bottomRight(); //右下点

    x = (tlPoint.x()+trPoint.x())/2;
    y = tlPoint.y();
    QPoint tcPoint = QPoint(x,y);

    x = (blPoint.x()+brPoint.x())/2;
    y = blPoint.y();
    QPoint bcPoint = QPoint(x,y);

    x = tlPoint.x();
    y = (tlPoint.y()+blPoint.y())/2;
    QPoint lcPoint = QPoint(x,y);

    x = trPoint.x();
    y = (trPoint.y()+brPoint.y())/2;
    QPoint rcPoint = QPoint(x,y);

    tlRect = QRect(tlPoint.x()-2,tlPoint.y()-2,6,6); //左上点
    trRect = QRect(trPoint.x()-2,trPoint.y()-2,6,6); //右上点
    blRect = QRect(blPoint.x()-2,blPoint.y()-2,6,6); //左下点
    brRect = QRect(brPoint.x()-2,brPoint.y()-2,6,6); //右下点
    tcRect = QRect(tcPoint.x()-2,tcPoint.y()-2,6,6); //上中点
    bcRect = QRect(bcPoint.x()-2,bcPoint.y()-2,6,6); //下中点
    lcRect = QRect(lcPoint.x()-2,lcPoint.y()-2,6,6);//左中点
    rcRect = QRect(rcPoint.x()-2,rcPoint.y()-2,6,6); //右中点

    painter.fillRect(tlRect,color);
    painter.fillRect(trRect,color);
    painter.fillRect(blRect,color);
    painter.fillRect(brRect,color);
    painter.fillRect(tcRect,color);
    painter.fillRect(bcRect,color);
    painter.fillRect(lcRect,color);
    painter.fillRect(rcRect,color);
}

void CScreenShot::updateMouseShape(const QPoint &point)
{
    switch (currentShotState)
    {
        case initShot:
        case beginShot:
            setCursor(Qt::CrossCursor);
            break;
        case beginMoveShot:
            setCursor(Qt::OpenHandCursor);
            break;
        case finishShot:
        case finishMoveShot:
        case finishControl:
            if (getSelectedRect().contains(point))
            {
                setCursor(Qt::OpenHandCursor);
            }
            else
            {
                updateMoveControlMouseShape(getMoveControlState(point));
            }
            break;
        case beginControl:
            updateMoveControlMouseShape(controlValue); //调用函数对移动8个控制点进行鼠标状态的改变
        break;
        default:
            setCursor(Qt::ArrowCursor);
        break;
    }
}

void CScreenShot::updateMoveControlMouseShape(controlPointEnum controlValue)
{
  switch (controlValue)
  {
    case moveControl1:
    case moveControl5:
      setCursor(Qt::SizeFDiagCursor);
      break;
    case moveControl2:
    case moveControl6:
      setCursor(Qt::SizeVerCursor);
      break;
    case moveControl3:
    case moveControl7:
      setCursor(Qt::SizeBDiagCursor);
      break;
    case moveControl4:
    case moveControl8:
      setCursor(Qt::SizeHorCursor);
      break;
    default:
      setCursor(Qt::ArrowCursor);
      break;
  }
}

CScreenShot::controlPointEnum CScreenShot::getMoveControlState(const QPoint &point)
{
    CScreenShot::controlPointEnum result = moveControl0;
    if (currentShotState == initShot || currentShotState == beginShot)
    {
        result = moveControl0;
    }
    else if (tlRect.contains(point))
    {
        result = moveControl1;
    }
    else if (tcRect.contains(point))
    {
        result = moveControl2;
    }
    else if (trRect.contains(point))
    {
        result = moveControl3;
    }
    else if (rcRect.contains(point))
    {
        result = moveControl4;
    }
    else if (brRect.contains(point))
    {
        result = moveControl5;
    }
    else if (bcRect.contains(point))
    {
        result = moveControl6;
    }
    else if (blRect.contains(point))
    {
        result = moveControl7;
    }
    else if (lcRect.contains(point))
    {
        result = moveControl8;
    }
    else
    {
        result = moveControl0;
    }

    return result;
}

QRect CScreenShot::getMoveAllSelectedRect(void)
{
    QRect result;
    QPoint tmpBeginPoint,tmpEndPoint;
    int moveX,moveY;
    checkMoveEndPoint(); //对移动选区进行判断，当移动的选区超出边界，则停止移动
    moveX = moveEndPoint.x() - moveBeginPoint.x();
    moveY = moveEndPoint.y() - moveBeginPoint.y();
    tmpBeginPoint.setX(beginPoint.x() + moveX);
    tmpBeginPoint.setY(beginPoint.y() + moveY);
    tmpEndPoint.setX(endPoint.x() + moveX);
    tmpEndPoint.setY(endPoint.y() + moveY);

    result = getRect(tmpBeginPoint, tmpEndPoint);
    return result;
}

QRect CScreenShot::getMoveControlSelectedRect(void)
{
  int x,y,w,h;
  QRect rect = getRect(beginPoint,endPoint);
  QRect result;
  switch (controlValue)
  {
    case moveControl1:
        result = getRect(rect.bottomRight(),moveEndPoint);
        return result;
        break;
    case moveControl2:
        x = rect.x();
        y = getMinValue(moveEndPoint.y(),rect.bottomLeft().y());
        w = rect.width();
        h = qAbs(moveEndPoint.y() - rect.bottomRight().y());
        break;
    case moveControl3:
        result = getRect(rect.bottomLeft(),moveEndPoint);
        return result;
        break;
    case moveControl4:
        x = getMinValue(rect.x(),moveEndPoint.x());
        y = rect.y();
        w = qAbs(rect.bottomLeft().x() - moveEndPoint.x());
        h = rect.height();
        break;
    case moveControl5:
        result = getRect(rect.topLeft(),moveEndPoint);
        return result;
        break;
    case moveControl6:
        x = rect.x();
        y = getMinValue(rect.y(),moveEndPoint.y());
        w = rect.width();
        h = qAbs(moveEndPoint.y() - rect.topLeft().y());
        break;
    case moveControl7:
        result = getRect(moveEndPoint,rect.topRight());
        return result;
        break;
    case moveControl8:
        x = getMinValue(moveEndPoint.x(),rect.bottomRight().x());
        y = rect.y();
        w = qAbs(rect.bottomRight().x() - moveEndPoint.x());
        h = rect.height();
        break;
    default:
        result = getRect(beginPoint,endPoint);
        return result;
        break;
  }

  return QRect(x,y,w,h); //获取选区
}

int CScreenShot::getMinValue(int num1, int num2)
{
  return num1<num2?num1:num2;
}

void CScreenShot::drawSelectedPixmap(void)
{
    painter.drawRect(selectedRect); //画选中的矩形框
    shotPixmap = loadPixmap.copy(selectedRect);  //更新选区的Pixmap
    if (selectedRect.width() > 0 && selectedRect.height())
    {
        painter.drawPixmap(selectedRect.topLeft(),shotPixmap); //画选中区域的图片
    }
    draw8ControlPoint(selectedRect); //画出选区的8个控制点
}

void CScreenShot::drawXYWHInfo(void)
{
    int x,y;
    QColor color = QColor(239, 234, 228, 200);
    QRect rect;
    QString strTipsText;

    switch (currentShotState)
    {
        case beginShot:
        case finishShot:
        case beginMoveShot:
        case finishMoveShot:
        case beginControl:
        case finishControl:
            x = selectedRect.x() + 5;
            y = selectedRect.y() > infoHeight ? selectedRect.y()-infoHeight:selectedRect.y();
            rect = QRect(x, y, infoWidth, infoHeight);
            strTipsText = QString(tr(" 坐标信息\n x:%1 y:%2\n w:%3 h:%4")).arg(selectedRect.x(), 4).arg(selectedRect.y(),4)
            .arg(selectedRect.width(),4).arg(selectedRect.height(),4);
            painter.fillRect(rect, color);
            painter.setPen(QPen(Qt::black));//设置画笔的颜色为黑色
            painter.drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, strTipsText);
        break;
        default:
        break;
    }
}

void ScreenShotInit(void)
{
    pLocalCScreenShotInfo = new CScreenShot();
}

CScreenShot *GetScrenShotInfo(void)
{
    return pLocalCScreenShotInfo;
}

void CScreenShot::SetMainWinStatus(bool Status)
{
    isMainWinHide = Status;
}
