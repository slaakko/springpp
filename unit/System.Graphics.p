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

  Graphics = object
    native: pointer;
    dpiX, dpiY: real;
    constructor();
    procedure Clear(color: Color);
    procedure DrawLine(pen: Pen; s, e: Point);
    procedure DrawRectangle(pen: Pen; rectangle: Rect);
    function MeasureString(text: string; font: Font): Size;
    procedure DrawString(text: string; font: Font; brush: Brush; location: Point);
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
    procedure Draw(graphics: Graphics); virtual;
    function Bounds(): Rect; virtual;
  end;

  ShapeArray = array of Shape;

  CompoundShape = object(Shape)
    count: integer;
    components: ShapeArray;
    constructor();
    procedure Add(shape: Shape);
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

implementation

function MMToPixels(mm: real; dpi: real): integer;
begin
  MMToPixels := mm * dpi / inchMM;
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

constructor Graphics();
begin
end;

procedure Graphics.Clear(color: Color); external;
procedure Graphics.DrawLine(pen: Pen; s, e: Point); external;
procedure Graphics.DrawRectangle(pen: Pen; rectangle: Rect); external;
function Graphics.MeasureString(text: string; font: Font): Size; external;
procedure Graphics.DrawString(text: string; font: Font; brush: Brush; location: Point); external;

constructor Bitmap(x, y: integer);
begin
  this.x := x;
  this.y := y;
end;

function Bitmap.GetGraphics(): Graphics; external;

procedure Bitmap.Save(fileName: string); external;

constructor Shape();
begin
end;

procedure Shape.Draw(graphics: Graphics);
begin
end;

function Shape.Bounds(): Rect;
begin
  Bounds := new Rect();
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
