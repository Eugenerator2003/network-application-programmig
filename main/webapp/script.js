addEventListener("DOMContentLoaded", getAll);
document.getElementById("sendButton").addEventListener("click", postOrPut);
document.getElementById("backToCreation").addEventListener("click", backToCreation);
let dataBar = document.getElementById("data-bar");
document.getElementById("backToCreation").style.display = "none"
var editId = -1;

function postOrPut() {
    let sum = document.getElementById("sum").value;
    let inputDate = document.getElementById("date").value;
    let date = inputDate
    let type;

    let radioButtons = document.getElementsByName('type');

    for (let i = 0; i < radioButtons.length; i++) {
        if (radioButtons[i].checked) {
            type = radioButtons[i].value;
            break
        }
    }

    if (type == undefined || date == "" || sum == "") {
        alert("Incorrect input data")
        return
    }

    let obj = { 'id' : window.editId, 'sum' : sum, 'date' : date, 'type' : type };
    $.ajax({
        url: document.URL + 'receipts',
        type: window.editId == -1 ? 'POST' : 'PUT',
        dataType: 'json',
        contentType: 'application/json', // Указываем тип контента как JSON
        data: JSON.stringify(obj), // Преобразуем объект в JSON-строку
        success: function (data) {
            if (editId == -1) {
                obj.id = data.id
                addRow(obj)
            }
            else {
                console.log(window.editId)
                updateRow(document.getElementById(window.editId.toString()), obj)
                window.editId = -1;
                backToCreation();
            }
            clearInput();
        },
        error: function (error) {
            console.log('Ошибка запроса: ', error)
        }
    });
}

function backToCreation() {
    let backToCreation = document.getElementById("backToCreation");
    backToCreation.style.display = "none";
    document.getElementById("sendButton").innerText = "Create"
    document.getElementById("operationName").innerText = "Adding"
    editId = -1;
}

function clearInput() {
    document.getElementById("sum").value = 0;
    document.getElementById("date").value = "";
}

function setEdit(id) {
    window.editId = id;
    console.log('edited id to ' + window.editId)
    let backToCreation = document.getElementById("backToCreation");
    backToCreation.style.display = "block";
    document.getElementById("operationName").innerText = "Editing";
    document.getElementById("sendButton").innerText = "Edit";
    let row = document.getElementById(id);
    let values = row.getElementsByTagName("td");
    document.getElementById("sum").value = values[1].innerText;
    document.getElementById("date").value = values[2].innerText;

    let radioButtons = document.getElementsByName('type');

    for(let i = 0; i < radioButtons.length; i++) {
        if (radioButtons[i].value == values[0].innerText.replace(" ", "")) {
            radioButtons[i].checked = true
            break;
        }
    }
}

function del(id) {
    $.ajax({
        url: document.URL + 'receipts?id=' + id,
        type: 'DELETE',
        success: function (data) {
            delRow(id);
        },
        error: function (error) {
            console.log('Ошибка запроса: ', error)
        }
    });
}

function getAll() {
    $.ajax( {
         url: document.URL + 'receipts',
         type: 'GET',
         dataType: 'json',
         success: function (data) {
             for (let entity of data.data) {
                 addRow(entity)
             }
         },
         error: function (error) {
             console.log('Ошибка запроса: ', error)
         }
        }
    );
}

function addRow(entity) {
    let thead = document.getElementById("row-entities");
    let row = document.createElement("tr")
    row.id = entity.id;
    let typeTd = document.createElement("td");
    let sumTd = document.createElement("td");
    let dateTd = document.createElement("td");
    let editTd = document.createElement("td");
    let removeTd = document.createElement("td");
    editTd.className = "edit-td"
    editTd.innerHTML = "<span onclick='setEdit(" + entity.id +  ")'>Edit</span>"
    removeTd.className = "delete-td"
    removeTd.innerHTML = "<span onclick='del("+ entity.id +")'>Delete</span>";
    row.appendChild(typeTd);
    row.appendChild(sumTd);
    row.appendChild(dateTd);
    row.appendChild(editTd)
    row.appendChild(removeTd)
    updateRow(row, entity);
    thead.appendChild(row);
}

function updateRow(row, entity) {
    console.log(row)
    let tds = row.getElementsByTagName("td");
    if (entity.type == "PublicUtilities") {
        entity.type = "Public Utilities"
    }
    tds[0].innerText = entity.type;
    tds[1].innerText = entity.sum;
    tds[2].innerText = entity.date
}

function delRow(id) {
    let row = document.getElementById(id);
    let thead = document.getElementById("row-entities");
    thead.removeChild(row)
}