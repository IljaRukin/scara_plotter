# scara_plotter
scara parallel arm robot plotter with ESP8266 and data transfer over wifi

# files
- "send data to ESP" includes code to send data over GET and POST method with iPython-Notebook to Arduino to toggle a LED
- "esp8266_plotter_v1" and "esp8266_plotter_v2" includes the arduino code for version 1 and 2 of the plotter (the only difference are the movement calculations)
- "send plotter data" includes python code to send a path to be drawn. first the svg is converted to .hpgl using incscape. afterwards a iPython-Notebook sends the coordinates in batches of continuous lines. between these batches the pen is moved up.

here are some images:<br>
<img src="/img/scara robot v1 (1).jpg" style="width:20%;">
<img src="/img/scara robot v2 (1).jpg" style="width:20%;">

and some video links: \
[<img src="https://img.youtube.com/vi/4elHrwVsAtQ/hqdefault.jpg" width="20%">](https://youtu.be/4elHrwVsAtQ)
[<img src="https://img.youtube.com/vi/mxgUuI1V4_M/hqdefault.jpg" width="20%">](https://youtu.be/mxgUuI1V4_M)

## version 1
- 2 servos for movement of the two arms and 1 for up/down of whole plattform

### issues
- unstable 
- up/down movement seized often up
- bad pen mount

## version 2
- sturdier construction
- more powerful servos
- improved pen mount and direct up/down control using small servo
- more complex kinematics, since the pen isn't mounted directly on the corner of the parallelogram

### issues
- servo accuracy is quite bad (especially visible when drawing chess board pattern in https://youtu.be/mxgUuI1V4_M)

## kinematics
<img src="/img/scara kinematics.jpg" style="width:20%;">
<img src="/img/scara range.bmp" style="width:20%;">
the scara robot discribed here operates in "working mode -+". \
note: the bottom image is from: https://www.semanticscholar.org/paper/Development-of-a-Five-Bar-Parallel-Robot-With-Large-Campos-Bourbonnais/4f945b3db879c18c8fdf2b01860d8a9b2f9a274d
