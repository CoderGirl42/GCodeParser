%
O0001 (Program Name)
G21 (Metric Units)
G90 (Absolute Positioning)
G00 X10 Y10 Z3.14 (Rapid Move to X10, Y10, Z2)
G01 Z0 F50 (Linear Move to Z0 at feedrate 50)
G01 X20 (Linear Move to X20)
G01 Y20 (Linear Move to Y20)
G01 X10 (Linear Move back to X10)
G01 Y10 (Linear Move back to Y10)
G00 Z10 (Rapid Move to Z10 - lift the tool)
N1 G00 X10 Y20 *45
M30 (Program End and Reset)
%
