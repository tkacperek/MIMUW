(* Tomasz Kacperek 334578 *)
(* Zadanie 5: Przelewanka *)

(* wstepnie sprawdza, czy dla podanych danych mozna uzyskac poprawny wynik *)
(* xy - niepusta tablica par int * int *)
let czy_mozliwe xy =
	try
		(* przynajmniej jedna szklanka musi byc pusta lub pelna *)
		begin
			Array.iter 
				(fun (x, y) -> 
					if x <> 0 && (y = 0 || y = x)
					then raise Exit
					else ())
				xy;
			false
		end
	with Exit ->
		let rec licz_nwd x y =
			if x = 0 then y else licz_nwd (y mod x) x
		in
		let nwd = 
			Array.fold_left
				(fun acc (x, _) -> licz_nwd acc x)
				0
				xy
		in
		try
			(* ilosc wody w szklankach zawsze jest podzielna 
			   przez nwd pojemnosci *)
			begin
				Array.iter
					(fun (_, y) -> 
						if y mod nwd <> 0
						then raise Exit
						else ())
					xy;
				true
			end
		with Exit -> false

(* backtracking *)
(* kolejka nie moze byc pusta *)
let backtracking historia kolejka x y n =
	let (stan, kroki) = Queue.pop kolejka
	in
	(* sprawdza, czy dany stan zostal wczesniej otrzymany *)
	if Hashtbl.mem historia stan then - 1
	else
	(* sprawdza, czy dany stan jest szukanym wynikiem *)
	if stan = y then kroki
	else
	begin
		(* dodaje stan do historii *)
		Hashtbl.add historia stan true;
		
		(* dodaje do kolejki nowe stany przez: *)

		(* napelnianie pustych szklanek *)
		for i = 0 to n - 1 do
			if stan.(i) = 0 then
				let nowy = Array.copy stan
				in
				begin
					nowy.(i) <- x.(i);
					Queue.push (nowy, kroki + 1) kolejka
				end
		done;

		(* oproznianie pelnych szklanek *)
		for i = 0 to n - 1 do
                        if stan.(i) = x.(i) then
                                let nowy = Array.copy stan
                                in
                                begin
                                        nowy.(i) <- 0;
                                        Queue.push (nowy, kroki + 1) kolejka
                                end
                done;

		(* przelewanie wody ze szklanek niepustych do szklanek niepelnych *)
		for i = 0 to n - 1 do
			for j = 0 to n - 1 do
				if i <> j && stan.(i) <> 0 && stan.(j) <> x.(j)
				then
					let nowy = Array.copy stan
					and ile = min (x.(j) - stan.(j)) (stan.(i))
					in
					begin
						nowy.(i) <- nowy.(i) - ile;
						nowy.(j) <- nowy.(j) + ile;
						Queue.push (nowy, kroki + 1) kolejka
					end
			done
		done;
		- 1
	end

(* oblicza ile czynnosci wystarczy do otrzymania danego stanu *)
let przelewanka xy =
	(* ile szklanek *)
	let n = Array.length xy
	in
	if n = 0 then 0
	else
		(* rozdzielenie xy na 2 tablice: 
			x - pojemnosci 
			y - stanu koncowego *)
		let x = Array.init n (fun i -> fst xy.(i))
		and y = Array.init n (fun i -> snd xy.(i))
		in
		(* jezeli wszystkie maja byc pelne, to zlicza niezerowe pojemnosci *)
		if x = y
		then Array.fold_left
			(fun acc a -> if a <> 0 then acc + 1 else acc)
			0
			x
		else
			if not (czy_mozliwe xy) then -1
			else
				(* przygotowanie do backtrackingu *)
				let historia = Hashtbl.create 100000
				and kolejka = Queue.create ()
				and wynik = ref (- 1)
				and start = Array.make n 0
				in
				begin
					(* zaczyna z pustymi szklankami *)
					Queue.push (start, 0) kolejka;
					(* backtracking / bfs *)
					while (not (Queue.is_empty kolejka)) && (!wynik = - 1) do
						wynik := backtracking historia kolejka x y n
					done;
					!wynik;
				end
