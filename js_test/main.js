var id = null;
var stopped = true;
var vel_x = 3;
var vel_y = 3;

function button_click_callback() {
  stopped = !stopped;
  change_button_color();
  if (!stopped)
    move();
}

function move()
{
  var elem = document.getElementById('image_animate');
  var pos_x = parseInt(elem.style.left) || 10;
  var pos_y = parseInt(elem.style.top) || 10;
  // var adding_x = 1;
  // var adding_y = 1;
  clearInterval(id);
  id = setInterval(frame, 10);
  function frame(){
    if (pos_x + elem.offsetWidth >= window.screen.width || pos_x <= 0)
    {
      vel_x*=(-1);
      flip_image(vel_x/Math.abs(vel_x));
    }
    if (pos_y + elem.offsetHeight >= window.screen.height || pos_y <= 0)
    {
      vel_y*=(-1);
    }
    if (!stopped)
    { pos_x += vel_x;
      pos_y += vel_y;
      elem.style.top = pos_y + 'px';
      elem.style.left = pos_x + 'px';
    }
  }
}

function change_button_color()
{
  var button = document.getElementById("button_id");
  if (!stopped)
    button.style.background = 'green';
  else 
    button.style.background = 'red';
}

function flip_image(arg)
{
  var image = document.getElementById("image_animate");
  var flip = 'scaleX(' + arg + ')';
  console.log(flip);
  image.style.transform = String(flip);
}