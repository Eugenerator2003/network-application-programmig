window.addEventListener("load", getAllData)
document.getElementById("get_button").addEventListener("click", getDistance)

function getAllData() {
    $.ajax({
        url: document.URL + "data",
        method: "get",
        dataType: "json",
        success: function(data) {
            setData(data)
        }
    });
}

function setData(data) {
    console.log(data.data)
    let div = document.getElementById("data-div")
    data.data.forEach((elem) => {
        let p = document.createElement("p")
        p.textContent = "Начало: " + elem.source + " Конец: " + elem.destination + " Вес: " + elem.weight
        p.className = "p-edge"
        div.appendChild(p)
    })
}

function getDistance() {
    let value = get_parameters_for_request()
    let params = "?source=" + value.source + "&?destination=" + value.destination
    $.ajax({
        url: document.URL + "data" + params,
        type: 'post',
        success: function (data) {
            let blob = new Blob([data], { type: 'application/octet-stream' });
            let link = document.createElement("a")
            link.href = window.URL.createObjectURL(blob)
            link.download = "result.txt"
            link.click()
        }
    });
}

function get_parameters_for_request() {
    let src = document.getElementById("source").value
    let dst = document.getElementById("destination").value
    let value = { source : src, destination : dst};
    console.log(value)
    return value;
}