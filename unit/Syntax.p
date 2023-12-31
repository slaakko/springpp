unit Syntax;

interface

uses System.Graphics.Primitive, System.Graphics;

type
  SyntaxShape = object(CompoundShape)
    bounds: Rect;
    padding: Padding;
    constructor();
    constructor(padding: Padding);
    procedure Measure(graphics: Graphics); override;
    function Bounds(): Rect; override;
    function Left(): Point; virtual;
    function Top(): Point; virtual;
    function Right(): Point; virtual;
    function Bottom(): Point; virtual;
    function ContentOrigin(): Point; virtual;
  end;

  RuleBox = object(SyntaxShape)
    ruleName: string;
    constructor(ruleName: string);
    procedure Measure(graphics: Graphics); override;
    procedure Draw(graphics: Graphics); override;
  end;

  Terminal = object(SyntaxShape)
    terminal: string;
    terminalSize: Size;
    constructor(terminal: string);
    function ContentOrigin(): Point; override;
    procedure Measure(graphics: Graphics); override;
    procedure Draw(graphics: Graphics); override;
  end;

var 
  font, boldFont: Font;
  noPadding: Padding;
  ruleBoxPadding: Padding;
  terminalPadding: Padding;

implementation

constructor SyntaxShape(padding: Padding);
begin
  bounds := new Rect();
  this.padding := padding;
end;

constructor SyntaxShape() : this(noPadding);
begin
end;

procedure SyntaxShape.Measure(graphics: Graphics);
begin
  base.Measure(graphics);
  bounds.size.w := bounds.size.w + padding.Horizontal();
  bounds.size.h := bounds.size.h + padding.Vertical();
end;

function SyntaxShape.Bounds(): Rect; 
begin
  Bounds := bounds;
end;

function SyntaxShape.Left(): Point; 
begin
  Left := new Point(bounds.location.x, bounds.location.y + bounds.size.h / 2);
end;

function SyntaxShape.Top(): Point; 
begin
  Top := new Point(bounds.location.x + bounds.size.w / 2, bounds.location.y);
end;

function SyntaxShape.Right(): Point; 
begin
  Right := new Point(bounds.location.x + bounds.size.w, bounds.location.y + bounds.size.h / 2);
end;

function SyntaxShape.Bottom(): Point; 
begin
  Bottom := new Point(bounds.location.x + bounds.size.w / 2, bounds.location.y + bounds.size.h);
end;

function SyntaxShape.ContentOrigin(): Point; 
var
  contentOrigin: Point;
begin
  contentOrigin := new Point(bounds.location.x, bounds.location.y);
  contentOrigin.Offset(padding.left, padding.top);
  ContentOrigin := contentOrigin;
end;

constructor RuleBox(ruleName: string) : base(ruleBoxPadding);
begin
  this.ruleName := ruleName;
end;

procedure RuleBox.Measure(graphics: Graphics); 
var
  size: Size;
begin
  size := graphics.MeasureString(ruleName, font);
  bounds.SetSize(size);
  base.Measure(graphics);
end;

procedure RuleBox.Draw(graphics: Graphics); 
begin
  base.Draw(graphics);
  graphics.DrawString(ruleName, font, blackBrush, ContentOrigin());
  graphics.DrawRectangle(blackPen, bounds);
end;

constructor Terminal(terminal: string) : base(terminalPadding);
begin
  this.terminal := terminal;
end;

function Terminal.ContentOrigin(): Point; 
var
  x, y: real;
begin
  x := bounds.location.x + (bounds.size.w - terminalSize.w) / 2;
  y := bounds.location.y + padding.top;
  ContentOrigin := new Point(x, y);
end;

procedure Terminal.Measure(graphics: Graphics); 
var 
  max: real;
begin
  terminalSize := graphics.MeasureString(terminal, boldFont);
  max := Max(terminalSize.w, terminalSize.h);
  bounds.SetSize(new Size(max, max));
  base.Measure(graphics);
end;

procedure Terminal.Draw(graphics: Graphics); 
begin
  base.Draw(graphics);
  graphics.DrawString(terminal, boldFont, blackBrush, ContentOrigin());
  if terminal.Length = 1 then graphics.DrawEllipse(blackPen, bounds) else DrawRoundedRectangle(graphics, bounds);
end;

begin
  font := new Font('Arial', 11, regularFontStyle);
  boldFont := new Font('Arial', 11, boldFontStyle);
  noPadding := new Padding(0, 0, 0, 0);
  ruleBoxPadding := new Padding(0.5, 0.5, 0.5, 0.5);
  terminalPadding := new Padding(0.5, 0.5, 0.5, 0.5);
end.
