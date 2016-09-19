(*
    Autor: Tomasz Kacperek (334578)
    Zadanie 0: Arytmetyka przybliżonych wartości
*)

(*  moduł realizujący przedział liczb rzeczywistych *)
module Przedzial : sig
	type przedzial
	(*konstruktor*)
	val p_nowy : float -> float -> przedzial
	(*selektory*)
	val p_od : przedzial -> float
	val p_do : przedzial -> float
	(*inne*)
	val czyNan : float -> bool
    end =
    struct
	(*  przedział pusty to (nan,nan) *)
	type przedzial = float * float
	(*  sprawdzania czy liczba jest nan'em *)
	let czyNan (x:float) = not (x=x);;
	let p_nowy x y =
	    (*	test wyłapujący (nan,a),(inf,inf) itp. *)
	    if czyNan (x-.y) then (nan,nan)
	    else if x>y then (y,x) else (x,y)
	let p_od (x,_) = x
	let p_do (_,y) = y
    end;;
include Przedzial;;

(*  niepusta lista rozłącznych przedziałów uporządkowanych
    relacją mniejszości w zbiorze R *)
type wartosc = przedzial list;;

(*KONSTRUKTORY*)

(*  w_nowa x y := [(x,y)] *)
let w_nowa x y = (p_nowy x y)::[];;

(*  tworzy wartość z przedziału *)
let w_z_p p = p::[];;

let wartosc_od_do = w_nowa;;

(*  p>0 *)
let wartosc_dokladnosc x p = w_nowa
    (x*.((100.-.p) /. 100.))
    (x*.((100.+.p) /. 100.));;

let wartosc_dokladna x = w_nowa x x;;

(*SELEKTORY*)

(*  konwertuje wartość na przedział *)
(*  podana wartość musi składać się z 1 przdziału *)
let p_z_w w =
    match w with
     h::[] -> h
    |[] | _::_ -> failwith "przekazano zla wartosc!";;

(*  sprawdza czy "x" jest w "w" *)
let in_wartosc w x =
    List.exists (fun a -> (p_od a) <= x && x <= (p_do a)) w;;

(*  min_wartosc w = najmniejsza możliwa wartość w *)
(*  lub neg_infinity jeśli brak dolnego ograniczenia *)
let min_wartosc w =
    match w with
	 [] -> failwith "pusta wartosc!"
	|h::_ ->
	    let x=p_od h in
	    if czyNan x then neg_infinity
	    else x;;

(*  max_wartosc w = największa możliwa wartość w *)
(*  lub infinity jeśli brak górnego ograniczenia *)
let max_wartosc w =
    match (List.rev w) with
	 [] -> failwith "pusta wartosc!"
	|h::_ ->
	    let x=p_do h in
	    if czyNan x then infinity
	    else x;;

(*  środek przedziału od min_wartosc do max_wartosc *)
(*  lub nan jeśli min lub max_wartosc nie są określone *)
let sr_wartosc w =
    let kandydat = (min_wartosc w +. max_wartosc w) /. 2.
    (*	test na [neg_]infinity *)
    in if czyNan (kandydat *. 0.) then nan
    else kandydat;;

(*MODYFIKATORY*)

(*  suma dwóch elementów typu wartosc (suma zbiorów) *)
let suma w1 w2 =
    let rec pom a b wyn =
	match a,b with
	 [],[] -> wyn
	|h::t,[] | [],h::t -> pom t [] (h::wyn)
	|h1::t1,h2::t2 ->
	    (*	A=[x1,y1],B=[x2,y2] *)
	    let x1 = p_od h1 and y1 = p_do h1
	    and x2 = p_od h2 and y2 = p_do h2 in
	    (*	pomiń nan'y *)
	    if czyNan (x1+.y1) then pom t1 b wyn
	    else if czyNan (x2+.y2) then pom a t2 wyn
	    (*	czy A=B *)
	    else if x1=x2 && y1=y2 then pom t1 t2 (h1::wyn)
	    (*	ustaw A "przed" B *)
	    else if x1>x2 then pom b a wyn
	    (*	czy AnB=0 *)
	    else if y1<x2 then pom t1 b (h1::wyn)
	    (*	czy BcA *)
	    else if y2<=y1 then pom a t2 wyn
	    (*	zamień B na AuB *)
	    else pom t1 (
		(p_nowy (min x1 x2) (max y1 y2))::t2) wyn
    in List.rev (pom w1 w2 []);;

(*  wykonuje operację "op" na każdej parze przedziałów
    z w1 i w2, a następnie zwraca sumę otrzymanych wartości *)
let kazdy_z_kazdym op w1 w2 =
    let rec pom a b wyn =
	match a,b with
	 [],_ -> wyn
	|_::t,[] -> pom t w2 wyn
	|h1::t1,h2::t2 -> pom a t2 (suma wyn (op h1 h2))
    in pom w1 w2 [];;

let plus a b = kazdy_z_kazdym (
    fun x y -> w_nowa (p_od x +. p_od y) (p_do x +. p_do y)
) a b;;

let rec razy w1 w2 =
    (*	przyda się do mnożenia przez -1 *)
    let m1 = (p_nowy (-1.) (-1.)) in
    let rec pom x y =
	(*  A=[a,b], B=[c,d] *)
	let a=p_od x and b=p_do x and c=p_od y and d=p_do y in
	if a=b then
	    (*	a*B *)
	    if a>=0. then w_nowa (a*.c) (a*.d)
	    (*	(-a)*(-B) *)
	    else pom
		    (p_nowy (-1.*.a) (-1.*.a))
		    (p_nowy (-1.*.d) (-1.*.c))
	(*  A*b=b*A *)
	else if c=d then pom y x
	(*  jeśli a,b różnych znaków to [a,b]=[a,0]u[0,b] *)
	else if a*.b<0. then
	    suma 
		(pom (p_nowy a 0.) y)
		(pom (p_nowy 0. b) y)
	(*  jeśli c,d różnych znaków to A*B=B*A *)
	else if c*.d<0. then pom y x
	(*  A,B po różnych stronach zera *)
	else if (a+.b)*.(c+.d)<0. then
	    (*	jeśli a,b<0,c,d>0 to A*B=-1*((-1*A)*B)) *)
	    if a+.b<0. then
		razy (w_z_p m1) (pom (p_z_w (pom m1 x)) y)
	    (*	wpp: A*B=B*A *)
	    else pom y x
	(*  a,b,c,d>0 *)
	else if a+.b>0. then w_nowa (a*.c) (b*.d)
	(*  jeśli a,b,c,d<0 to A*B=(-1*A)*(-1*B) *)
	else pom (p_z_w (pom m1 x)) (p_z_w (pom m1 y))
    in kazdy_z_kazdym pom w1 w2;;

(*  a-b=a+(-b) *)
let minus a b = plus a (razy (w_nowa (-1.) (-1.)) b);;

let podzielic w1 w2 =
    (*	przyda się do odwracania *)
    let jeden = p_nowy 1. 1. in
    let rec pom x y =
	(*  A=[a,b], B=[c,d] *)
	let a=p_od x and b=p_do x and c=p_od y and d=p_do y in
	(*  A/0=nan *)
	if c=0. && d=0. then w_nowa nan nan
	(*  A/B=A*(1/B) *)
	else if a<>1. || b<>1. then
	    razy (w_z_p x) (pom jeden y)
	(*  od teraz A=[1,1] *)
	(*  c,d różnych znaków *)
	else if c*.d<0. then
	   suma
		(pom jeden (p_nowy c 0.))
		(pom jeden (p_nowy 0. d))
	(*  "zwyczajna" odwrotność *)
	else if c*.d>0. then w_nowa (1./.d) (1./.c)
	(*  oddzielam kolejne 2 przypadki, aby uniknąć
	    niejasności związanych z  0., -0., +0 *)
	(*  1/[0,d] *)
	else if c=0. then w_nowa (1./.d) infinity
	(*  1/[c,0] *)
	else w_nowa neg_infinity (1./.c)
    in kazdy_z_kazdym pom w1 w2;;
