program Stx;

uses System.Graphics.Primitive, System.Graphics, Syntax;

var
  x, y: integer;
  bm: Bitmap;
  expressionRule: RuleBox;
  terminal: Terminal;
  graphics: Graphics;

begin
  x := MMToPixels(100, dpiX);
  y := MMToPixels(100, dpiY);
  bm := new Bitmap(x, y);
  graphics := bm.GetGraphics();
  graphics.Clear(white);
  expressionRule := new RuleBox('expression');
  expressionRule.Measure(graphics);
  expressionRule.bounds.location := new Point(10, 10);
  expressionRule.Draw(graphics);
  terminal := new Terminal(';');
  terminal.Measure(graphics);
  terminal.bounds.location := new Point(10, 20);
  terminal.Draw(graphics);
  bm.Save('C:/work/springpp/prog/Stx.png');
end.
