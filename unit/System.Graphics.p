unit System.Graphics;

interface

uses System.Graphics.Primitive;

const 
  dpiX: real = 96;
  dpiY: real = 96;
  inchMM: real = 25.4;
  pointMM: real = 0.351450;
  regularFontStyle: integer = 0;
  boldFontStyle: integer = 1;
  italicFontStyle: integer = 2;
  underlineFontStyle: integer = 4;
  strikeOutFontStyle: integer = 8;

type
  PointArray = array of Point;

  Color = object
    a, r, g, b: Byte;
    constructor(a, r, g, b: Byte);
  end;

  Pen = object
    color: Color;
    width: real;
    constructor(color: Color);
    constructor(color: Color; width: real);
  end;

  Brush = object 
    color: Color;
    constructor(color: Color);
  end;

  Font = object
    family: string;
    size: real;
    style: integer;
    constructor(family: string; size: real; style: integer);
  end;

  Padding = object
    left, top, right, bottom: real;
    constructor(left, top, right, bottom: real);
    function Horizontal(): real;
    function Vertical(): real;
  end;

  Graphics = object
    native: pointer;
    dpiX, dpiY: real;
    constructor();
    procedure Clear(color: Color);
    procedure DrawLine(pen: Pen; s, e: Point);
    procedure DrawRectangle(pen: Pen; rectangle: Rect);
    function MeasureString(text: string; font: Font): Size;
    procedure DrawString(text: string; font: Font; brush: Brush; location: Point);
    procedure FillPolygon(brush: Brush; points: PointArray);
    procedure DrawEllipse(pen: Pen; rect: Rect);
    procedure DrawArc(pen: Pen; rect: Rect; startAngle, sweepAngle: real);
  end;

  Bitmap = object
    x, y: integer;
    native: pointer;
    constructor(x, y: integer);
    function GetGraphics(): Graphics;
    procedure Save(fileName: string);
  end;

  Shape = object
    constructor();
    procedure Measure(graphics: Graphics); virtual;
    procedure Draw(graphics: Graphics); virtual;
    function Bounds(): Rect; virtual;
  end;

  Line = object(Shape)
    routingPoints: PointArray;
    constructor(routingPoints: PointArray);
    procedure Draw(graphics: Graphics); override;
    function Bounds(): Rect; override;
  end;

  Arrow = object(Shape)
    routingPoints: PointArray;
    lineArrowWidth, lineArrowHeight: real;
    constructor(routingPoints: PointArray);
    procedure Draw(graphics: Graphics); override;
    function Bounds(): Rect; override;
  end;

  ShapeArray = array of Shape;

  CompoundShape = object(Shape)
    count: integer;
    components: ShapeArray;
    constructor();
    procedure Add(shape: Shape);
    procedure Measure(graphics: Graphics); override;
    procedure Draw(graphics: Graphics); override;
    function Bounds(): Rect; override;
  end;

var
  black: Color;
  white: Color;
  blackPen: Pen;
  blackBrush: Brush;

function MMToPixels(mm: real; dpi: real): integer;
function PixelsToMM(pixels: integer; dpi: real): real;
procedure DrawArrowLine(graphics: Graphics; s, e: Point; lineArrowWidth, lineArrowHeight: real);
procedure DrawRoundedRectangle(graphics: Graphics; rect: Rect);

implementation

function MMToPixels(mm: real; dpi: real): integer;
var
  pixels: integer;
begin
  pixels := integer(mm * dpi / inchMM);
  MMToPixels := pixels;
end;

function PixelsToMM(pixels: integer; dpi: real): real;
begin
  PixelsToMM := pixels * inchMM / dpi;
end;

constructor Color(a, r, g, b: Byte);
begin
  this.a := a;
  this.r := r;
  this.g := g;
  this.b := b;
end;

constructor Pen(color: Color);
begin
  this.color := color;
  this.width := PixelsToMM(1, dpiX);
end;

constructor Pen(color: Color; width: real);
begin
  this.color := color;
  this.width := width;
end;

constructor Brush(color: Color);
begin
  this.color := color;
end;

constructor Font(family: string; size: real; style: integer);
begin
  this.family := family;
  this.size := size;
  this.style := style;
end;

constructor Padding(left, top, right, bottom: real);
begin
  this.left := left;
  this.top := top;
  this.right := right;
  this.bottom := bottom;
end;

function Padding.Horizontal(): real;
begin
  Horizontal := left + right;
end;

function Padding.Vertical(): real;
begin
  Vertical := top + bottom;
end;

constructor Graphics();
begin
end;

procedure Graphics.Clear(color: Color); external;
procedure Graphics.DrawLine(pen: Pen; s, e: Point); external;
procedure Graphics.DrawRectangle(pen: Pen; rectangle: Rect); external;
function Graphics.MeasureString(text: string; font: Font): Size; external;
procedure Graphics.DrawString(text: string; font: Font; brush: Brush; location: Point); external;
procedure Graphics.FillPolygon(brush: Brush; points: PointArray); external;
procedure Graphics.DrawEllipse(pen: Pen; rect: Rect); external;
procedure Graphics.DrawArc(pen: Pen; rect: Rect; startAngle, sweepAngle: real); external;

constructor Bitmap(x, y: integer);
begin
  this.x := x;
  this.y := y;
end;

function Bitmap.GetGraphics(): Graphics; external;
procedure Bitmap.Save(fileName: string); external;

procedure DrawArrowLine(graphics: Graphics; s, e: Point; lineArrowWidth, lineArrowHeight: real);
var
  arrowLine, arrowStartLine, arrowEndLine, leftArrowLine, rightArrowLine: LineSegment;
  av, uv, la, ra: Vector;
  a: real;
  points: PointArray;
begin
  arrowLine := new LineSegment(e, s);
  av := arrowLine.ToVector();
  a := lineArrowWidth * Sqrt(3) / 2;
  uv := Product(a, Unit(av));
  arrowStartLine := new LineSegment(e, uv);
  arrowEndLine := new LineSegment(arrowStartLine.e, e);
  la := Product(lineArrowHeight / 2, Unit(RotateLineSegment(arrowEndLine, 90.0).ToVector()));
  ra := Product(lineArrowHeight / 2, Unit(RotateLineSegment(arrowEndLine, -90.0).ToVector()));
  leftArrowLine := new LineSegment(arrowEndLine.s, la);
  rightArrowLine := new LineSegment(arrowEndLine.s, ra);
  points := new Point[3];
  points[0] := e;
  points[1] := leftArrowLine.e;
  points[2] := rightArrowLine.e;
  graphics.FillPolygon(blackBrush, points);
  graphics.DrawLine(blackPen, arrowEndLine.s, arrowLine.e);
end;

procedure DrawRoundedRectangle(graphics: Graphics; rect: Rect);
begin
  { todo }
end;

constructor Shape();
begin
end;

procedure Shape.Measure(graphics: Graphics); 
begin
end;

procedure Shape.Draw(graphics: Graphics);
begin
end;

function Shape.Bounds(): Rect;
begin
  Bounds := new Rect();
end;

constructor Line(routingPoints: PointArray);
begin
  this.routingPoints := routingPoints;
end;

procedure Line.Draw(graphics: Graphics); 
var 
  i, n: integer;
  s, e: Point;
begin
  n := routingPoints.Length;
  if n > 1 then
  begin
    for i := 1 to n - 1 do
    begin
      s := routingPoints[i - 1];
      e := routingPoints[i];
      graphics.DrawLine(blackPen, s, e);
    end;
  end;
end;

function Line.Bounds(): Rect; 
var
  i, n: integer;
  p: Point;
  minX, maxX, minY, maxY: real;
begin
  minX := 9999999999;
  maxX := -1;
  minY := 9999999999;
  maxY := -1;
  n := routingPoints.Length;
  if n > 0 then
  begin
    for i := 0 to n - 1 do
    begin
      p := routingPoints[i];
      minX := Min(p.x, minX);
      maxX := Max(p.x, maxX);
      minY := Min(p.y, minY);
      maxY := Max(p.y, maxY);
    end;
    Bounds := new Rect(new Point(minX, minY), new Size(maxX - minX, maxY - minY));
  end else Bounds := new Rect();
end;

constructor Arrow(routingPoints: PointArray);
begin
  this.routingPoints := routingPoints;
  this.lineArrowWidth := 3;
  this.lineArrowHeight := 2;
end;

procedure Arrow.Draw(graphics: Graphics); 
var 
  i, n: integer;
  s, e: Point;
begin
  n := routingPoints.Length;
  if n > 1 then
  begin
    for i := 1 to n - 1 do
    begin
      s := routingPoints[i - 1];
      e := routingPoints[i];
      if i < n - 1 then graphics.DrawLine(blackPen, s, e) else DrawArrowLine(graphics, s, e, lineArrowWidth, lineArrowHeight);
    end;
  end;
end;

function Arrow.Bounds(): Rect; 
var
  i, n: integer;
  p: Point;
  minX, maxX, minY, maxY: real;
begin
  minX := 9999999999;
  maxX := -1;
  minY := 9999999999;
  maxY := -1;
  n := routingPoints.Length;
  if n > 0 then
  begin
    for i := 0 to n - 1 do
    begin
      p := routingPoints[i];
      minX := Min(p.x, minX);
      maxX := Max(p.x, maxX);
      minY := Min(p.y, minY);
      maxY := Max(p.y, maxY);
    end;
    Bounds := new Rect(new Point(minX, minY), new Size(maxX - minX, maxY - minY));
  end else Bounds := new Rect();
end;

constructor CompoundShape();
begin
  count := 0;
  components := new Shape[4];
end;

procedure CompoundShape.Add(shape: Shape);
var
  newLength, i: integer;
  newComponents: ShapeArray;
begin
  if count = components.Length then
  begin
    if components.Length < 4 then newLength := 4 else newLength := 2 * components.Length;
    newComponents := new Shape[newLength];
    for i := 0 to count - 1 do
    begin
      newComponents[i] := components[i];
    end;
    components := newComponents;
  end;
  components[count] := shape;
  count := Succ(count);
end;

procedure CompoundShape.Measure(graphics: Graphics); 
var 
  i: integer;
  s: Shape;
begin
  for i := 0 to count - 1 do
  begin
    s := components[i];
    s.Measure(graphics);
  end;
end;

procedure CompoundShape.Draw(graphics: Graphics);
var 
  i: integer;
  s: Shape;
begin
  for i := 0 to count - 1 do
  begin
    s := components[i];
    s.Draw(graphics);
  end;
end;

function CompoundShape.Bounds(): Rect; 
var 
  i: integer;
  s: Shape;
  bounds, r: Rect;
begin
  bounds := new Rect();
  for i := 0 to count - 1 do
  begin
    s := components[i];
    r := s.Bounds();
    if not r.IsEmpty() then
    begin
      if bounds.IsEmpty() then bounds := r else bounds := Union(bounds, r);
    end;
  end;
  Bounds := bounds;
end;

begin
  black := new Color(Byte(255), Byte(0), Byte(0), Byte(0));
  white := new Color(Byte(255), Byte(255), Byte(255), Byte(255));
  blackPen := new Pen(black);
  blackBrush := new Brush(black);
end.
