:- module(main, [sum_age/2, max_age_of_hobby/3, person_in_range/4]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE


% sum_age(NameList, TotalAge).
sum_age([], 0).
sum_age([First | Rest], TotalAge) :-
    person(First, Age, Hobby),
    sum_age(Rest, RestTotalAge),
    TotalAge is Age + RestTotalAge.


% max_age_of_hobby(NameList, Hobby, MaxAge).
max_age_of_hobby([], Hobby, 0).
max_age_of_hobby([First | Rest], Hobby, Age) :-
    person(First, Age, Hobby),
    max_age_of_hobby(Rest, Hobby, RestMaxAge),
    Age > RestMaxAge.
max_age_of_hobby([First | Rest], Hobby, RestMaxAge) :-
    person(First, Age, Hobby),
    max_age_of_hobby(Rest, Hobby, RestMaxAge),
    Age =< RestMaxAge.
max_age_of_hobby([First | Rest], Hobby, RestMaxAge) :-
    person(First, Age, DifferentHobby),
    Hobby \= DifferentHobby,
    max_age_of_hobby(Rest, Hobby, RestMaxAge).


% person_in_range(NameList, Min, Max, ResultList).
person_in_range([], Min, Max, []).
person_in_range([First | Rest], Min, Max, [First | RestResultList]) :-
    person(First, Age, Hobby),
    Min =< Age, Age =< Max,
    person_in_range(Rest, Min, Max, RestResultList).
person_in_range([First | Rest], Min, Max, RestResultList) :-
    person(First, Age, Hobby),
    (Min > Age; Age > Max),
    person_in_range(Rest, Min, Max, RestResultList).
