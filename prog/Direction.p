program Direction;

type
  Direction = (noDir, north, east, south, west);

var
  d: Direction;

begin
  d := noDir;
  if d = noDir then Writeln('noDir') else Writeln('other');
end.
