var conn = new WebSocket('ws://te-auxilium.se:8585');

conn.onopen = function(e) {
    console.log("Connection established!");
    conn.send(JSON.stringify({type: "client_connect", msg:""}));
};

conn.onmessage = function(e) {
    console.log(JSON.parse(e.data));
    let type=JSON.parse(e.data).type;
    console.log(type);
    //switch()
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
})