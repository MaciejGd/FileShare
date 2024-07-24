var id = null;
function move() {
  var elem = document.getElementById('image_animate');
  var pos_x = parseInt(elem.style.left) || 10;
  var pos_y = parseInt(elem.style.top) || 10;
  var adding_x = 1;
  var adding_y = 1;
  clearInterval(id);
  id = setInterval(frame, 10);
  function frame(){
    if (pos_x + elem.offsetWidth >= window.screen.width || pos_x <= 0)
    {
      adding_x*=(-1);
    }
    if (pos_y + elem.offsetHeight >= window.screen.height || pos_y <= 0)
    {
      adding_y*=(-1);
    }
    pos_x += adding_x;
    pos_y += adding_y;
    elem.style.top = pos_y + 'px';
    elem.style.left = pos_x + 'px';
  }
}

// var id = null;
// function move() {
//     var elem = document.getElementById('image_animate');
//     var pos_x = parseInt(elem.style.left) || 0; // Initialize with 0 if style.left is not set
//     var pos_y = parseInt(elem.style.top) || 0;  // Initialize with 0 if style.top is not set
//     var adding_x = 1;
//     var adding_y = 1;
//     var elem_width = elem.offsetWidth;
//     var elem_height = elem.offsetHeight;
//     clearInterval(id);
//     id = setInterval(frame, 10);

//     function frame() {
//         if ((pos_x + elem_width >= window.innerWidth && adding_x > 0) || (pos_x <= 0 && adding_x < 0)) {
//             adding_x *= -1;
//         }
//         if ((pos_y + elem_height >= window.innerHeight && adding_y > 0) || (pos_y <= 0 && adding_y < 0)) {
//             adding_y *= -1;
//         }
//         pos_x += adding_x;
//         pos_y += adding_y;
//         elem.style.top = pos_y + 'px';
//         elem.style.left = pos_x + 'px';
//     }
// }
