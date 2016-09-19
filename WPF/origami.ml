(*  Tomasz Kacperek 334578 *)
(*  Zadanie: Origami *)

(*  reprezentacja punktu *)
type point = float * float;;

(*  typ mówiący "ile warstw przebije szpilka w danym punkcie" *)
type kartka = point -> int;;

(*  typ reprezentujący orientację punktu względem prostej *)
type strona = Lewa | Srodek | Prawa;;

(*  zwraca prostokątną kartkę *)
let prostokat (lewo, dol) (prawo, gora) = function (x, y) ->
	if lewo <= x && x <= prawo && dol <= y && y <= gora then 1 else 0;;

(*  zwraca okrągłą kartkę *)
let kolko (xp, yp) r =
    let kwadrat x = x *. x
    in function (x, y) ->
	if kwadrat (x -. xp) +. kwadrat (y -. yp) <= kwadrat r then 1 else 0;;

(*  składa kartkę k wzdłóż prostej zadanej punktami *)
let zloz (x1, y1) (x2, y2) k =
    (*	sprawdza po której stronie prostej znajduje się punkt *)
    let gdzie =
	(*  prosta równoległa do osi OY *)
	if x1 = x2 then
	    let (mniej, wiecej) =
		if y2 < y1 then (Prawa, Lewa) else (Lewa, Prawa)
	    in function (x, _) -> if x < x1 then mniej
				    else if x = x1 then Srodek else wiecej
	(*  sprawdza czy punkt jest nad / pod prostą *)
	else function (x, y) -> 
		let w = (y1 -. y2) *. (x -. x2) +. (x1 -. x2) *. (y2 -. y)
		in if w < 0. then Prawa else if w = 0. then Srodek else Lewa
    (*	oblicza współrzędne punktu symetrycznego do punktu względem prostej *)
    and symetryczny = function (x, y) ->
	(*  proste równoległe do osi OX / OY *)
	if x1 = x2 then (2. *. x1 -. x, y)
	else if y1 = y2 then (x, 2. *. y1 -. y)
	else(*	współczynniki podanej prostej i prostej prostopadłej *)
	    let a = (y1 -. y2) /. (x1 -. x2)
	    and ap = (x2 -. x1) /. (y1 -. y2) in
		(*  wyrazy wolne podanej prostej i prostej
		    przechodzącej przez podany punkt *)
		let b = y2 -. a *. x2
		and bp = y -. ap *. x in
		    (*	przecięcie prostych *)
		    let xc = (bp -. b) /. (a -. ap) in
		    let yc = a *. xc +. b in
			(*  punkt symetryczny *)
			(2. *. xc -. x, 2. *. yc -. y)
    (*	"złożenie" kartki *)
    in function p -> match gdzie p with
	    (*	po prawej stronie pusto *)
	     Prawa -> 0
	    (*	na zgięciu "po staremu" *)
	    |Srodek -> k p
	    (*	po lewej "po staremu" + symetrycznie "po staremu" *)
	    |Lewa -> k p + k (symetryczny p);;

(*  wielokrotny zloz; przyjmuje listę punktów reprezentujących proste *)
let skladaj l k = List.fold_left (fun acc (p1, p2) -> zloz p1 p2 acc) k l;; 
