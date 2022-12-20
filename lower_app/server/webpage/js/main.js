// @ts-nocheck
import {createApp} from 'vue'

//config for axios default
//axios.defaults.baseURL='http://192.168.204.1:8080'  //用于跨域访问
axios.defaults.timeout=2*1000

//设置硬件的模块
createApp({
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
        }
    }
}).mount('#app');

