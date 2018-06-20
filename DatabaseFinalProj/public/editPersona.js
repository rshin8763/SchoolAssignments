document.addEventListener("DOMContentLoaded", myFunctions);

function fillForm(){
    var req = new XMLHttpRequest();
    req.open("GET", "/select-id?tName=arcana", true);
    req.addEventListener('load', function(){
        if(req.status >=200 && req.status <400){
            var response = JSON.parse(req.responseText);
            var form = document.getElementById('aid');
            for (var i=0; i < response.length; i++){
                var option = document.createElement('option')
                option.setAttribute('value', response[i].id)
                option.textContent = response[i].name
                form.appendChild(option);
            }
        } else {
            console.log("Error in network request: " + req.statusText);
        }
    });
    req.send(null);
    event.preventDefault();
}

function updateEntry() {
    var req = new XMLHttpRequest();

    var name = document.getElementById('name').value;
    var lvl = document.getElementById('lvl').value;
    var aid = document.getElementById('aid').value;
    var id = document.getElementById('id').value;

    req.open("GET", "/update-persona?name="+name+"&lvl="+lvl+"&aid="+aid+"&id="+id, true);
    req.addEventListener('load', function(){
            if(req.status >=200 && req.status <400){
            location.href='/persona';
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
        fillForm();
        var tName = document.getElementById('tName').textContent;
        fillTable(tName);
    }
}


function fillTable(tName){
    var req = new XMLHttpRequest();
    var id = document.getElementById('id').value;
    req.open("GET", "/get-persona?id="+id, true);
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
            }
        } else {
            console.log("Error in network request: " + req.statusText);
        }
    });
    req.send(null);
    event.preventDefault();
}
