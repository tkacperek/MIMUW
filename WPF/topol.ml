(*  Autor: Tomasz Kacperek 334578 *)
(*  Zadanie 4: Sortowanie topologiczne *)

open PMap

(*  wyjatek jest podnoszony ( :-P ) gdy zostanie znaleziony cykl *)
exception Cykliczne

(*  sortowanie topologiczne *)
let topol lista_s =
    (*	mapa sasiedztwa: klucz -> lista kluczy sasiadow *)
    let mapa_s = List.fold_left (fun m (k, s) -> add k s m) empty lista_s in
    (*	dodaje do mapy k(lucz) oraz s(asiadow) ze stopniem wejsciowym *)
    let pom k s m =
	(*  nowa mapa po [nie] dodaniu k (klucza) *)
	let nm = if mem k m then m else add k (ref 0) m in
	(*  dodaje klucz do mapy lub zwieksza jego stopien *)
	let pom2 ma kl =
	    (*	jezeli kl(ucz) jest w ma(pie) *)
	    if mem kl ma then
		(*  to zwiesza jego stopien wejsciowy *)
		let st = find kl ma in
		begin
		    st := !st + 1;
		    ma
		end
	    (*	wpp dodaje go ze stopniem = 1 *)
	    else add kl (ref 1) ma in
	(*  dodaje sasiadow do mapy *)
	List.fold_left pom2 nm s in
    (*	mapa stopni: klucz -> stopien wejsciowy *)
    let mapa_st = ref (foldi pom mapa_s empty) in
    (*	tu bedzie (odwrocony) wynik sortowania *)
    let wynik = ref [] in
    (*  klucze o zerowym stopniu wejsciowym *)
    let usun = ref [] in
    (*  przyda sie przy dodawaniu nowych kluczy do usuniecia *)
    let usun2 = ref [] in
    (*	dodaje k(lucz) do wyniku i sprzata po nim *)
    let pom3 k =
	begin
	    (*	usuwa k(lucz) z mapy stopni *)
	    mapa_st := remove k !mapa_st;
	    (*	jezeli klucz mial sasiadow *)
	    if mem k mapa_s then
		(*   to zmniejsza ich stopien o 1 *)
		List.iter
		    (fun kl ->
                        let x = find kl !mapa_st in
                        x := !x - 1;
                        (*  i dopisuje klucze o wyzerowanym stopniu do usun2 *)
                        if !x = 0 then usun2 := kl::(!usun2)
                    )
		    (find k mapa_s);
	    (*	dopisuje k(lucz) do wyniku *)
	    wynik := k::(!wynik)
	end in
    begin
        (*  dodaje do usun klucze o zerowym stopniu *)
        usun := foldi
                (fun k st l -> if !st = 0 then k::l else l)
                !mapa_st
                [];
	(*  dopoki zostaly nieprzetworzone klucze *)
	while not (is_empty !mapa_st) do
	    (*	jezeli nie ma kluczy o stopniu = 0, to znaleziono cykl *)
	    if !usun = [] then raise Cykliczne else
		(*  wpp usuwa wybrane elementy *)
                List.iter pom3 !usun;
                (*  i porzadkuje listy *)
                usun := !usun2;
                usun2 := []
	done;
	(*  zwraca odwrocony wynik, czyli prawidlowy wynik :-) *)
	(List.rev !wynik)
    end
