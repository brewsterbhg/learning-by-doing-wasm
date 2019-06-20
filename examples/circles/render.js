// Set up the canvas and context
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

/**
 * Render function to call on every animation frame
 *
 * @param {Number} dataLength - The number of circles multiplied by their number of properties
 * @param {Number} circleStructSize - The number of circle properties (for parsing Array Buffer data)
 */
const render = (dataLength, circleStructSize) => {
  // Create Int32Array for reading from C memory
  let circles = new Int32Array(
    Module.HEAP8.buffer, // The buffer data from C heap memory
    _getCircles(canvas.width, canvas.height), // The C function to handle our circle animation logic
    dataLength // The total length of data the buffer should contain (circles * struct size)
  );

  // Clear the canvas
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // Loop over the circle data and render the circles
  for (let i = 0; i < circles.length; i += circleStructSize) {
    // Get the circle properties
    const [x, y, rad, r, g, b] = circles.slice(i, i + circleStructSize);

    // Draw the circle
    ctx.beginPath();
    ctx.arc(x, y, rad, 0, 2 * Math.PI, false);
    ctx.fillStyle = `rgba(${r},${g},${b},1)`;
    ctx.fill();
  }

  // When the animation frame has finished, call render again
  window.requestAnimationFrame(() => render(dataLength, circleStructSize));
};
