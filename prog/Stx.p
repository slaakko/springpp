program Stx;

uses System.Graphics.Primitive, System.Graphics, Syntax;

var
  x, y: integer;
  bm: Bitmap;
  graphics: Graphics;
  title: DiagramTitle;
  expressionRule: RuleBox;
  terminal: Terminal;
  terminal2: Terminal;
  keyword: Keyword;

begin
  x := MMToPixels(100, dpiX);
  y := MMToPixels(100, dpiY);
  bm := new Bitmap(x, y);
  graphics := bm.GetGraphics();
  graphics.Clear(white);
  title := new DiagramTitle('constant-expression');
  title.Measure(graphics);
  title.bounds.location := new Point(10, 10);
  title.Draw(graphics);
  expressionRule := new RuleBox('expression');
  expressionRule.Measure(graphics);
  expressionRule.bounds.location := new Point(10, 20);
  expressionRule.Draw(graphics);
  terminal := new Terminal(';');
  terminal.Measure(graphics);
  terminal.bounds.location := new Point(10, 30);
  terminal.Draw(graphics);
  terminal2 := new Terminal('<=');
  terminal2.Measure(graphics);
  terminal2.bounds.location := new Point(10, 40);
  terminal2.Draw(graphics);
  keyword := new Keyword('keyword');
  keyword.Measure(graphics);
  keyword.bounds.location := new Point(10, 50);
  keyword.Draw(graphics);
  bm.Save('C:/work/springpp/prog/Stx.png');
end.
