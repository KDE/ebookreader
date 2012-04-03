// toolbar.js

// Calculate item width to fill the width of the container while maintaining
// minimum item width
function calcTabWidth() {
    var margin = 1
    var containerWidth = parent.width
    var itemCount = views.length
    var itemWidth = Math.floor(containerWidth / itemCount) >= minimumItemWidth ?
            Math.floor(containerWidth / itemCount) : minimumItemWidth;
    itemWidth -= margin //needed in order to display completely all buttons
    return itemWidth;
}
