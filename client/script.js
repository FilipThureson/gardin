var conn = new WebSocket('ws://te-auxilium.se:8585');

conn.onopen = function(e) {
    console.log("Connection established!");
    conn.send(JSON.stringify({type: "client_connect", msg:""}));
};

conn.onmessage = function(e) {
    let msg=JSON.parse(e.data).msg;
    let type=JSON.parse(e.data).type;
    console.log(type);
    switch(type){
        case "send_state":
            if(msg=="down"){
                document.querySelector("#state").innerHTML=" Nere";
                document.querySelector("#down").style.display="none";
                document.querySelector("#up").style.display="inline";
            }else if(msg=="up"){
                document.querySelector("#state").innerHTML=" Uppe";
                document.querySelector("#up").style.display="none";
                document.querySelector("#down").style.display="inline";
            }

            break;
        case "server_connect":
            conn.send(JSON.stringify({type: "client_connect", msg:""}));

            break;
    }
};

document.querySelector("#up").addEventListener("click", function(){
    conn.send(JSON.stringify({
        type:"client_action", 
        msg:"up"
    }))
});

document.querySelector("#down").addEventListener("click", function(){
    conn.send(JSON.stringify({
        type:"client_action", 
        msg:"down"
    }))
});