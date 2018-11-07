open SharedTypes

type values = {
  first: int,
  second: int
};

type status =
  | Loading
  | DisplayPokemon(array(pokemon))
  | ChoosingPokemon
  | Failure;

type state = {
  firstInput: int,
  secondInput: int,
  status: status,
}

type input =
  | FirstInput
  | SecondInput;

type action =
  | Nothing
  | ChangeInput(int, input)
  | Submit
  | LoadedPokemon(array(pokemon))
  | LoadPokemonFailed;

let url = "http://pokeapi.salestock.net/api/v2/pokemon/";

module Decode = {
  let pokemon = pokemon =>
    Json.Decode.{
      name: field("name",string, pokemon),
      id: field("id", int, pokemon),
      spriteUrl: at(["sprites", "front_default"], string, pokemon),
    };
  let pokemons = json => Json.Decode.list(pokemon, json);
};


let fetchPokemon = (id: int) =>
  Js.Promise.(
    Fetch.fetch(url++string_of_int(id))
    |> then_(Fetch.Response.json)
    |> then_(json =>
        json |> Decode.pokemon |> (pokemon => Some(pokemon) |> resolve))
  );

let component = ReasonReact.reducerComponent("Home")

let make = _children => {
  ...component,
  initialState: () => {
    firstInput: 0,
    secondInput: 0,
    status: ChoosingPokemon,
  },
  reducer: (action, state: state) =>
    switch action {
    | Nothing => ReasonReact.Update(state)
    | ChangeInput(newValue, FirstInput) => ReasonReact.Update({...state, firstInput: newValue})
    | ChangeInput(newValue, SecondInput) => ReasonReact.Update({...state, secondInput: newValue})
    | Submit => ReasonReact.UpdateWithSideEffects({...state, status: Loading}, (self =>
      Js.Promise.(
      all([|fetchPokemon(state.firstInput), fetchPokemon(state.secondInput)|])
      |> then_(([|p1, p2|]) =>
        switch (p1, p2) {
        | (Some(p1), Some(p2)) => resolve(self.send(LoadedPokemon([|p1, p2|])))
        | _ => resolve(self.send(LoadPokemonFailed))
        }
      )
      |> ignore)))
    | LoadedPokemon(pokemons) => ReasonReact.Update({...state, status: DisplayPokemon(pokemons)})
    | LoadPokemonFailed => ReasonReact.Update({...state, status: Failure})
    },
  render: self =>
    switch self.state.status {
    | ChoosingPokemon =>
      <div >
        <label>(ReasonReact.string("First Pokemon"))
          <input type_="number" onChange={event => self.send(ChangeInput(ReactEvent.Form.target(event)##value, FirstInput))}/>
        </label>
        <label>(ReasonReact.string("Second Pokemon"))
          <input type_="number" onChange={event => self.send(ChangeInput(ReactEvent.Form.target(event)##value, SecondInput))} />
        </label>
        <button onClick=((_event) => self.send(Submit))>(ReasonReact.string("I Choose You"))</button>
      </div>
    | DisplayPokemon(pokemons) =>
      <div>(
        Array.map(
          (pokemon) => <Pokemon pokemon key=string_of_int(pokemon.id) />, pokemons
        )
        |> ReasonReact.array
        )
      </div>
    | Failure =>
      <div>(ReasonReact.string("Failure"))</div>
    | Loading =>
      <div>(ReasonReact.string("Loading..."))</div>
  },
}
