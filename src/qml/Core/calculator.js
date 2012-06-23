var lastOp = ""

function doOperation(op) {
    if (op.toString().length==1 && (op >= "0" && op <= "9") ) {
        if (display.text.toString().length >= 14) {
            return; // No arbitrary length numbers
        }
        if (lastOp.toString().length == 0) {
            display.text = op.toString()
        } else if (lastOp >= "0" && lastOp <= "9" ) {
            display.text = display.text + op.toString()
        }
        lastOp = op
        return
    }
    lastOp = op

    if (op == "OK") {
        display.setPage(display.text)
    } else if (op == "DEL") {
        display.text = ""
        lastOp = ""
    } else if (op == "First") {
        display.text = "1"
    } else if (op == "Middle") {
        var mid = Math.ceil(display.nbPages/2)
        display.text = mid.toString()
    } else if (op == "Last") {
        display.text = display.nbPages.toString()
    }
}
