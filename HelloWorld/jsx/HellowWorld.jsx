
var helloWorldDll = null;

function initExtension() {
    try {
        helloWorldDll = new ExternalObject("lib:" + "C:\Users\\tjerf\\source\\repos\\HelloWorld\\x64\\Debug\\dll\\HelloWorld.dll");
    } catch (e) {
        alert("exception: " + e);
    }
}

initExtension();
function showAlertMessage(index) {
    // Acquire message corresponding to number (index)
    var message = helloWorldDll.extGetAlertMessage(index);
    alert(message);
}
function showAlertMessageDefault() {
    // Acquire default message
    var message = helloWorldDll.extGetAlertMessageDefault();
    alert(message);
}

showAlertMessage(1);