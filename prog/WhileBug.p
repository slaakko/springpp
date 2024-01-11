program WhileBug;

type
  SyntaxRow = object
    constructor();
  end;

  Component = object
    constructor();
  end;

var
  s: SyntaxRow;

constructor SyntaxRow();
begin
end;

constructor Component();
begin
end;

procedure Add(count: integer);
begin
end;

function Bug(): SyntaxRow;
var
  row, count: integer;
  component: Component;
  syntaxRow: SyntaxRow;
begin
  row := 0;
  count := 0;
  component := new Component();
  while row >= count do
  begin
    Add(count);
  end;
  syntaxRow := SyntaxRow(component);
  Bug := syntaxRow;
end;

begin
  s := Bug();
end.
