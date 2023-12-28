program Point;
type
  Point = object
    x, y: real;
    constructor(x, y: real);
  end;
var
  p: Point;

constructor Point(x, y: real);
begin
  this.x := x;
  this.y := y;
end;

begin
  p := new Point(1, 2);
end.
