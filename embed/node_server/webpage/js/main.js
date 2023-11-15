// @ts-nocheck

//config for axios default
//axios.defaults.baseURL='http://192.168.204.1:8080'  //用于跨域访问
axios.defaults.timeout=500

//设置硬件的模块
Vue.createApp({
    data(){
        return {
            message:"Information!",
            isShow:true,
            intervalId:null,
            deviceInfo:{
                led:"off",
                beep:"off",
                ir:"0",
                als:"0",
                ps:"0",
                gypox:"0",
                gypoy:"0",
                gypoz:"0",
                accelx:"0",
                accely:"0",
                accelz:"0",
                temp:"0",
                angle:"0",
            }
        }   
    },
    methods:{
        dev_set: function(device, action){
            axios({
                method:"post",
                url:"axiosDeviceSet",
                params:{
                    dev:device,
                    set:action
                }
            }).then(res=>console.log(res)).catch(err=>console.log(err));
            this.message = device+ " " + action;
        },

        changeShow: function(val){
            if(val == 0)
                this.isShow = true;
            else
                this.isShow = false;
        },

        dataRefresh: function(){
            let t = this;
            if(this.intervalId != null){
                return;
            }
            this.intervalId = setInterval(() => {
                axios({
                    method:"get",
                    url:"axiosDeviceGet",
                }).then(res=>{
                    console.log(res.data)
                    for (let key in res.data){
                        if(t.deviceInfo.hasOwnProperty(key)){
                            t.deviceInfo[key] = res.data[key]
                        }
                    }
                }).catch(err=>console.log(err));
            }, 1000);
        },

        clear: function(){
            clearInterval(this.intervalId);
            this.intervalId = null;
        }
    },
    created(){
        this.dataRefresh();
    },
    destroyed(){
        this.clear();
    }
}).mount("#app");

