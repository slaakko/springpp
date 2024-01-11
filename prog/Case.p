program Case;
type
  Direction = (north, east, south, west);
var
  dir: Direction;  
  index: integer;
begin
  index := 0;
  repeat
    if index = 0 then dir := north else if index = 1 then dir := east else if index = 2 then dir := south else dir := west;
    case dir of 
      north: Writeln('north', ' ', dir); 
      east: Writeln('east', ' ', dir);
      south: Writeln('south', ' ', dir);
      west: Writeln('west', ' ', dir) 
    end;
    index := Succ(index);
  until index >= 4;
end.
