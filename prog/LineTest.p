program LineTest;
uses 
  System.Graphics.Primitive;
var
  line: Line;
  rotated: Line;
begin
  line := new Line(new Point(0, 0), new Point(1, 0));
  line.Print();
  rotated := RotateLine(line, 90);
  rotated.Print();
end.
