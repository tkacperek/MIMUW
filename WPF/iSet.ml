(*  Autor: Tomasz Kacperek 334578 *)
(*  Zadanie 3: Modyfikacja drzew *)

(*  typ zbioru przedzialowego *)
type t =
    | Empty
    (*  Node: poddrzewo, para, poddrzewo, wysokosc, ilosc liczb w poddrzewach *)
    | Node of t * (int * int) * t * int * int

(*  zwraca pusty zbior *)
let empty = Empty

(*  sprawdza, czy zbior jest pusty *)
let is_empty set = set = Empty

(*  zwraca liczbe elementow w zbiorze *)
let getCount = function
    | Empty -> 0
    | Node (_, (kp, kk), _, _, _) -> abs (kk - kp) + 1

(*  zwraca wysokosc drzewa-zbioru *)
let getHeight = function
    | Empty -> 0
    | Node (_, _, _, h, _) -> h

(*  zwraca ilosc liczb w poddrzewach *)
let getBelow = function
    | Empty -> 0
    | Node (_, _, _, _, b) -> b

(*  zwraca "bezpieczna sume int'ow", ktora nie wychodzi poza zakres *)
let safeSum a b =
    if a < 0 && b < 0 && a <= min_int - b then min_int
    else if a >= 0 && b >= 0 && a >= max_int - b then max_int
    else a + b

(*  zwraca drzewo zrobione z dwoch drzew i przedzialu-klucza *)
(*  l, k, r rozlaczne *)
let make l k r = Node (l, k, r, max (getHeight l) (getHeight r) + 1,
    safeSum (safeSum  (getBelow l) (getCount l))
	    (safeSum (getBelow r) (getCount r)))

(*  porzadek w zbiorze przedzialow *)
let cmpI (a, b) (c, d) =
    (*  (a, b) mniejszy *)
    if b < c then -1
    (*  (c, d) mniejszy *)
    else if d < a then 1
    (*  nieporownywalne <-> niepuste przeciecie *)
    else 0

(*  porzadek w zbiorze niesasiadujacych przedzialow *)
let cmpSep (a, b) (c, d) =
    (*  (a, b) mniejszy *)
    if safeSum b 1 < c then -1
    (*  (c, d) mniejszy *)
    else if safeSum d 1 < a then 1
    (*	nieporownywalne *)
    else 0

(*  sprawdza, czy x jest w set *)
let mem x set =
    let rec loop = function
	| Empty -> false
	| Node (l, k, r, _, _) ->
	    let c = cmpI (x, x) k in
	    (*	x jest w k lub sprawdz w lewym / prawym poddrzewie *)
	    c = 0 || loop (if c < 0 then l else r) in
    loop set

(*  wywojuje f z kazdym przedzialem z set *)
let iter f set =
    let rec loop = function
	| Empty -> ()
	| Node (l, k, r, _, _) -> loop l; f k; loop r in
    loop set

(*  zwraca [(f xN ... (f x2 (f x1 a))...)] *)
(*  x'y to przedzialy uporzadkowane rosnaca *)
let fold f set acc =
    let rec loop acc = function
	| Empty -> acc
        | Node (l, k, r, _, _) ->
	    loop (f k (loop acc l)) r in
    loop acc set

(*  zwraca liste przedzialow uporzadkowanych rosnaco *)
let elements set =
    let rec loop acc = function
	| Empty -> acc
	| Node (l, k, r, _, _) -> loop (k :: loop acc r) l in
    loop [] set

(*  zwraca ilosc liczb niewiekszych od x w zbiorze set *)
let below x set =
    let rec loop set = match set with
	| Empty -> 0
	| Node (l, k, r, _, _) -> 
	    let c = cmpI (x, x) k in
	    (*	jesli x jest w k *)
	    if c = 0 then
		(*  zlicza liczby w lewym poddrzewie oraz te, ktore sa <= x w k *)
		let kp = fst k in
		safeSum (safeSum (getBelow l) (getCount l)) 
			(if kp = min_int then safeSum (safeSum x max_int) 2
			 else safeSum (safeSum x (-kp)) 1)
	    (*	jesli x < k, to szuka w lewym poddrzewie *)
	    else if c < 0 then loop l
	    (*	jesli k < x, to zlicza liczby w lewym i "szuka miejsca" x w prawym *)
	    else safeSum (safeSum (getBelow l) (getCount l))
			    (safeSum (getCount set) (loop r))
    in loop set

(*  balancer z pSet dopasowany do typu t *)
let bal l k r =
    let hl = getHeight l in
    let hr = getHeight r in
    if hl > hr + 2 then
	match l with
	    | Node (ll, lk, lr, _, _) ->
		if getHeight ll >= getHeight lr then make ll lk (make lr k r)
		else
		(match lr with
		    | Node (lrl, lrk, lrr, _, _) ->
			make (make ll lk lrl) lrk (make lrr k r)
		    | Empty -> assert false)
	    | Empty -> assert false
    else if hr > hl + 2 then
	match r with
	    | Node (rl, rk, rr, _, _) ->
		if getHeight rr >= getHeight rl then make (make l k rl) rk rr
		else
		(match rl with
		    | Node (rll, rlk, rlr, _, _) ->
			make (make l k rll) rlk (make rlr rk rr)
		    | Empty -> assert false)
	    | Empty -> assert false
	else make l k r

(*  dodaje do zbioru przedzial rozloczny ze wszystkimi przedzialami zbioru *)
(*  znajduje puste poddrzewo, w ktorym porzadek cmpI pozwala umiescic x *)
(*  nastepnie balansuje drzewo, przechodzac od liscia do korzenia *)
let rec addSep x = function
    | Empty -> Node (Empty, x, Empty, 1, 0)
    | Node (l, k, r, _, _) ->
	let c = cmpI x k in 
	(*  argument x niezgodny ze specyfikacja *)
	if c = 0 then invalid_arg "ISet.addSep"
	else if c < 0 then bal (addSep x l) k r
	else bal l k (addSep x r)

(*  laczy dwa rozlaczne zbiory oraz przedzial rozlaczny z tymi zbiorami *)
(*  l < k < r *)
let rec joinSep l k r =
    match (l, r) with
	| (Empty, _) -> addSep k r
	| (_, Empty) -> addSep k l
	| ( Node (ll, lk, lr, lh, _), Node (rl, rk, rr, rh, _) ) ->
	    (*	dodaje k do nizszego drzewa i laczy z drugim drzewem *)
	    if lh > rh + 2 then bal ll lk (joinSep lr k r)
	    else if rh > lh + 2 then bal (joinSep l k rl) rk rr
	    (*	lub dla drzew zrownowazonych (wzgledem siebie) *)
	    (*	po prostu "skleja" to, co trzeba *)
	    else make l k r

(*  laczy dwa zbiory-drzewa *)
(*  rekurencyjnie dolacza nizsze drzewo do poddrzewa wyzszego drzewa *)
let rec connect a b = match (a, b) with
    | (Empty, c) | (c, Empty) -> c
    | (Node (al, ak, ar, ah, _), Node (bl, bk, br, bh, _)) ->
	if ah > bh + 2 then
	    bal al ak (connect ar b)
	else
	    bal (connect a bl) bk br

(*  zwraca trojke (l,p,r) *)
(*  gdzie l - zbior elementow < x, p - czy x jest w set, r - zbior > x *)
let split x set =
    let rec loop = function
	| Empty -> (Empty, false, Empty)
	| Node (l, k, r, _, _) ->
	    let c = cmpI (x, x) k in
	    (*	jesli x jest w k, to *)
	    if c = 0 then
		let (kp, kk) = k in
		(*  lewy zbior to lewe poddrzewo z "kawalkiem" k *)
		((if kp = x then l else addSep (kp, x - 1) l), true,
		(*  prawy analogicznie *)
		(if kk = x then r else addSep (x + 1, kk) r))
	    (*	jesli x < k, to *)
	    else if c < 0 then
		(*  wywolaj sie dla lewego poddrzewa *)
		(*  i prawy wynik sklej z prawym poddrzewem *)
		let (ll, found, rl) = loop l in (ll, found, joinSep rl k r)
	    (*	dla k < x analogicznie *)
	    else let (lr, found, rr) = loop r in (joinSep l k lr, found, rr)
    in loop set

(*  usuwa przedzial x ze zbioru set *)
let remove (xp, xk) set =
    (*	"tnie" set w punkcie xp *)
    let (newl, _, _) = split xp set
    (*	oraz xk *)
    and (_, _, newr) = split xk set
    (*	i skleja to, co zostalo po wyrzuceniu srodka :-) *)
    in connect newl newr

(*  zwraca sume zbioru skladajacego sie z x oraz *)
(*  wszystkich przedzialow o niepustym przecieciu z x *)
let rec sumWithIntersecting x = function
	| Empty -> x
	| Node (l, k, r, _, _) ->
	    let c = cmpSep x k in
	    (*	jezeli jest niepuste przeciecie, to *)
	    if c = 0 then
		(*  wywolaj sie dla poddrzew *)
		let (inl, _) = sumWithIntersecting x l
		and (_, inr) = sumWithIntersecting x r
		and (xp, xk) = x
		and (kp, kk) = k in
		(*  i zwroc sume *)
		(min inl (min xp kp), max inr (max xk kk))
	    (*	wpp szukaj w odpowiednim poddrzewie *)
	    else if c < 0 then
		sumWithIntersecting x l
	    else
		sumWithIntersecting x r

(*  dodaje przedzial x do zbioru set *)
let add x set =
    (*	znajduje sume z przecieciami *)
    let x = sumWithIntersecting x set in
    (*	wyjmuje ja *)
    let set = remove x set in
    (*	teraz mozna latwo dodac ww. sume, poniewaz jest rozlaczna *)
    (*	z kazdym przedzialem w set *)
    addSep x set
