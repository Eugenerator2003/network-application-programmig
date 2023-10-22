window.addEventListener("load", get_all_data)
document.getElementById("get_button").addEventListener("click", get_distance)

function get_all_data() {
    $.ajax({
        url: document.URL + "data",
        method: "get",
        dataType: "json",
        success: function(data) {
            set_data(data)
        }
    });
}

function set_data(data) {
    console.log(data.data)
    let div = document.getElementById("data-div")
    data.data.forEach((elem) => {
        let p = document.createElement("p")
        p.textContent = "Начало: " + elem.source + " Конец: " + elem.destination + " Вес: " + elem.weight
        p.className = "p-edge"
        div.appendChild(p)
    })
}

function get_distance() {
    $.ajax({
        url: document.URL + "data",
        method: "get",
        data: get_parameters_for_request(),
        responseType: "arraybuffer",
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
    return {"source" : src, "destination" : dst};
}