document.addEventListener("DOMContentLoaded", myFunctions);

function fillForm(){
    var req1= new XMLHttpRequest();
    req1.open("GET", "/select-persona-id", true);
    req1.addEventListener('load', function(){
        if(req1.status >=200 && req1.status <400){
            var response = JSON.parse(req1.responseText);
            var form = document.getElementById('pid');
            for (var i=0; i < response.length; i++){
                var option = document.createElement('option')
                option.setAttribute('value', response[i].id)
                option.textContent = response[i].name
                form.appendChild(option);
            }
            var searchForm = document.getElementById('pidSearch')
            for (var i=0; i < response.length; i++){
                var option = document.createElement('option')
                option.setAttribute('value', response[i].id)
                option.textContent = response[i].name
                searchForm.appendChild(option);
            }
        } else {
            console.log("Error in network request: " + req1.statusText);
        }
    });
    req1.send(null);
    event.preventDefault();

    var req2= new XMLHttpRequest();
    req2.open("GET", "/select-skill-id", true);
    req2.addEventListener('load', function(){
        if(req2.status >=200 && req2.status <400){
            var response = JSON.parse(req2.responseText);
            var form = document.getElementById('sid');
            for (var i=0; i < response.length; i++){
                var option = document.createElement('option')
                option.setAttribute('value', response[i].id)
                option.textContent = response[i].name
                form.appendChild(option);
            }
        } else {
            console.log("Error in network request: " + req2.statusText);
        }
    });
    req2.send(null);
    event.preventDefault();
}

function fillTable(tName){
    var req = new XMLHttpRequest();
    req.open("GET", "/select-has_skill", true);
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


function addEntry(){
    var req = new XMLHttpRequest();

    var tName = document.getElementById('tName').textContent;
    var pid  = document.getElementById('pid').value;
    var sid  = document.getElementById('sid').value;
    var lvl = document.getElementById('lvl').value;

    req.open("GET", "/insert-has_skill?pid="+pid+"&sid="+sid+"&lvl="+lvl, true);
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

function searchSkills(){
    var req = new XMLHttpRequest();
    var tName = document.getElementById('tName').textContent;
    var pid  = document.getElementById('pidSearch').value;
    console.log(pid);

    req.open("GET", "/search-has_skill?pid="+pid, true);
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

function myFunctions(){
    if(document.getElementById('myTable'))
    {
        var tName = document.getElementById('tName').textContent;
        fillTable(tName);
        fillForm();
    }
}

