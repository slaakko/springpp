program Line;
uses
  System.Graphics.Primitive,
  System.Graphics;
var
  bm: Bitmap;
  x, y: integer;
  graphics: Graphics;
  font: Font;
  size: Size;
  textX, textY: integer;
begin
  x := MMToPixels(100, dpiX);
  y :=  MMToPixels(100, dpiY);
  bm := new Bitmap(x, y);
  graphics := bm.GetGraphics();
  graphics.Clear(white);
  graphics.DrawLine(blackPen, new Point(0, 0), new Point(100, 100));
  graphics.DrawRectangle(blackPen, new Rect(new Point(10, 10), new Size(80, 80)));
  font := new Font('Arial', 10, regularFontStyle);
  size := graphics.MeasureString('foobar', font);
  textX := 50 - size.w / 2;
  textY := 50 - size.h / 2;
  graphics.DrawString('foobar', font, blackBrush, new Point(textX, textY));
  bm.Save('C:/work/springpp/prog/line.png');
end.
