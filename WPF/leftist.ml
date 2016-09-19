(*  Autor: Tomasz Kacperek 334578 *)
(*  Zadanie 1: Drzewa lewicowe *)

(*  typ drzewa binarnego *)
(*  węzeł: priorytet, prawa wysokość *)
(*  prawa wysokość dla Null to 1 *)
type 'a tree = Null | Node of 'a * int * 'a tree * 'a tree;;

(*  typ złączalnej kolejki priorytetowej *)
type 'a queue = 'a tree;;

(*  konstruktor pustej kolejki *)
let empty = Null;;

(*  zwraca połączenie poprawnych drzew / kolejek *)
let rec join tree1 tree2 =
    match tree1, tree2 with
    (*	puste drzewo / kolejka jest elementem neutralnym łączenia *)
     Null, t | t, Null -> t
    |Node (prio1, rh1, leftSubTree, rightSubTree), Node (prio2,_,_,_) ->
	(* porządkowanie drzew / kolejek wg priorytetów *)
	if prio2 < prio1 then join tree2 tree1
	else
	    (*	połączenie prawego poddrzewa / kolejki z drugim drzewem / kolejką *)
	    let tree3 = join rightSubTree tree2 in
	    match leftSubTree, tree3 with
	    (*	Null jest neutralny *)
	     Null, t | t, Null -> Node (prio1, 1, t, Null)
	    (*	ustawienie drzewa / kolejki o mniejszej prawej wysokości jako prawego poddrzewa *)
	    |Node (_, rh_lST,_,_), Node (_, rh_t3,_,_) ->
		if rh_lST <= rh_t3 then Node (prio1, rh_lST + 1, tree3, leftSubTree)
		else Node (prio1, rh_t3 + 1, leftSubTree, tree3);;

(*  zwraca kolejkę z dołączonym elementem *)
let add x q = join (Node (x, 1, Null, Null) ) q;;

(*  wyjątek pustej kolejki *)
exception Empty;;

(*  zwraca pierwszy element i drzewo / kolejkę bez tego elementu *)
(*  lub podnosi wyjątek dla pustego drzewa / kolejki *)
let delete_min tree =
    match tree with
     Null -> raise Empty
    |Node (x,_, leftSubTree, rightSubTree) -> (x, join leftSubTree rightSubTree);;

(*  zwraca true dla pustej kolejki lub false dla niepustej *)
let is_empty q = q = Null;;
