document.addEventListener("DOMContentLoaded", myFunctions);

function fillForm(){
    var req = new XMLHttpRequest();
    req.open("GET", "/select-chara-id", true);
    req.addEventListener('load', function(){
        if(req.status >=200 && req.status <400){
            var response = JSON.parse(req.responseText);
            var form = document.getElementById('cid');
            for (var i=0; i < response.length; i++){
                var option = document.createElement('option')
                option.setAttribute('value', response[i].id)
                option.textContent = response[i].fname
                form.appendChild(option);
            }
        } else {
            console.log("Error in network request: " + req.statusText);
        }
    });
    req.send(null);
    event.preventDefault();
}

function fillTable(tName){
    var req = new XMLHttpRequest();
    req.open("GET", "/select-"+tName, true);
    req.addEventListener('load', function(){
        if(req.status >=200 && req.status <400){
            var response = JSON.parse(req.responseText);

            var table = document.getElementById('myTable');
            //clear table body.
            table.innerHTML = "";

            var header = table.createTHead();
            var body = table.createTBody();
            //set header
            var row = header.insertRow();
            for (prop in response[0]){
                if (prop != "id") {
                    var cell = row.insertCell();
                    cell.outerHTML  = "<th>"+ prop+ "</th>";
                }
            }
            
            for (var i=0; i < response.length; i++){
                row = body.insertRow();
                for (prop in response[i]) {
                    if (prop != "id") {
                        cell = row.insertCell();
                        cell.textContent = response[i][prop];
                    }
                }

                var curID = response[i]["id"];

                //add delete button
                cell = row.insertCell();
                cell.innerHTML = "<input type='button' value='delete' onclick='removeEntry("+ curID + ")'/>";
            }
        } else {
            console.log("Error in network request: " + req.statusText);
        }
    });
    req.send(null);
    event.preventDefault();
}


function removeEntry(id) {
    var req = new XMLHttpRequest();
    var tName = document.getElementById('tName').textContent;
    req.open("GET", "/delete?tName="+tName+"&id="+ id, true);
    req.addEventListener('load', function(){
            if(req.status >=200 && req.status <400){
            fillTable(tName);
            } else {
            console.log("Error in network request: " + req.statusText);
            }
            });
    req.send(null);
    event.preventDefault();
}

function addEntry(){
    var req = new XMLHttpRequest();

    var tName = document.getElementById('tName').textContent;
    var name = document.getElementById('name').value;
    var lvl  = document.getElementById('lvl').value;
    var cid = document.getElementById('cid').value;

    req.open("GET", "/insert-arcana?name="+name+"&lvl="+lvl+"&cid="+cid, true);
    req.addEventListener('load', function(){
        if(req.status >=200 && req.status <400){
            fillTable(tName);
        } else {
            console.log("Error in network request: " + req.statusText);
        }
    });
    req.send(null);
    event.preventDefault();
}

function myFunctions(){
    if(document.getElementById('myTable'))
    {
        var tName = document.getElementById('tName').textContent;
        fillTable(tName);
        fillForm();
    }
}

