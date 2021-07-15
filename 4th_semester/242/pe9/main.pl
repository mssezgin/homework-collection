:- module(main, [bigram/2, num_hobbies/2, sentence_match/2]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE


% bigram(Word, ResultBigram).
bigram(Word, Rbg) :- atom_chars(Word, Chars), bigraml(Chars, Rbg).

bigraml([F, S | _], Rbg) :- atom_chars(Rbg, [F, S]).
bigraml([_ | Rest], Rbg) :- bigraml(Rest, Rbg).



% hobby(HobbyName, NumberOfOccurrences).
% num_hobbies(NameList, ResultList).
num_hobbies([], []).
num_hobbies([Name | RestName], [hobby(Hob, Y) | RestRestResult]) :-
    person(Name, _, Hob),
    num_hobbies(RestName, RestResult),
    ((select(hobby(Hob, X), RestResult, RestRestResult), !, Y is X + 1);
    (not(select(hobby(Hob, _), RestResult, _)), RestRestResult = RestResult, Y = 1)).



% sentence_match(WordList, ResultList).
sentence_match(WordList, ResultList) :- sm(WordList, _, ResultList).


sm([], [], []).

sm([First1 | Rest1], Mappings, [Word | Rest2]) :-
    sm(Rest1, MappingsRest, Rest2),
    atom_chars(First1, Chars1),
    word(Word),
    atom_chars(Word, CharsW),
    match_word(Chars1, M, CharsW),
    expand(M, MappingsRest, Mappings).


match_word([], [], []).

match_word([F1 | R1], Maps, [F2 | R2]) :-
    match_word(R1, MapsRest, R2),
    ((member([F1, F2], MapsRest), Maps = MapsRest);
    (not(member([F1, _], MapsRest)), not(member([_, F2], MapsRest)), append([[F1, F2]], MapsRest, Maps))).


expand(Other, [], Other).

expand(Other, [First | Rest], Result) :-
    member(First, Other),
    expand(Other, Rest, Result).

expand(Other, [[A, B] | Rest], Result) :-
    not(member([A, _], Other)),
    not(member([_, B], Other)),
    append(Other, [[A, B]], NewOther),
    expand(NewOther, Rest, Result).
