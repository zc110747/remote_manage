// @ts-nocheck

//config for axios default
//axios.defaults.baseURL='http://192.168.204.1:8080'  //用于跨域访问
axios.defaults.timeout=2*1000

//设置硬件的模块
Vue.createApp({
    data(){
        return {
            message:"Information!",
            isShow:true,
            ledstatus:"ON",
            beepstatus:"ON",
            apx:0,
            apy:0,
            apz:0,
            gypox:0,
            gypoy:0,
            gypoz:0,
            accelx:0,
            accely:0,
            accelz:0
        }   
    },
    methods:{
        dev_set(device, action){
            axios({
                method:"post",
                url:"axiosDeviceSet",
                params:{
                    dev:device,
                    set:action
                }
            }).then(res=>console.log(res))
            .catch(err=>console.log(err))
            this.message = device+ " " + action;
        },
        changeShow(val)
        {
            if(val == 0)
                this.isShow = true;
            else
                this.isShow = false;
        }
    }
}).mount("#app");

