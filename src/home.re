type pokemon = {
  name: string,
  id: int,
  spriteUrl: string,
};
type values = {
  first: int,
  second: int
};

type status =
  | Loading
  | DisplayPokemon(pokemon)
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
  | LoadedPokemon(pokemon)
  | LoadPokemonFailed;

let url = "http://pokeapi.salestock.net/api/v2/pokemon/1";

module Decode = {
  let pokemon = pokemon =>
    Json.Decode.{
      name: field("name",string, pokemon),
      id: field("id", int, pokemon),
      spriteUrl: at(["sprites", "front_default"], string, pokemon),
    };
  let pokemons = json => Json.Decode.list(pokemon, json);
};


let fetchPokemon = () =>
  Js.Promise.(
    Fetch.fetchWithInit(url, Fetch.RequestInit.make(~mode=CORS,  ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}), ()))
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
    | Submit => ReasonReact.SideEffects((self => Js.Promise.(
      fetchPokemon()
      |> then_(result =>
        switch result {
        | Some(pokemon) => resolve(self.send(LoadedPokemon(pokemon)))
        | None => resolve(self.send(LoadPokemonFailed))
        }
      )
      |> ignore)))
    | LoadedPokemon(pokemon) => ReasonReact.Update({...state, status: DisplayPokemon(pokemon)})
    | LoadPokemonFailed => ReasonReact.Update({...state, status: Failure})
    },
  render: self =>
    switch self.state.status {
    | ChoosingPokemon =>
      <form onSubmit=((_event) => self.send(Submit))>
        <label>(ReasonReact.string("First Pokemon"))
          <input type_="number" onChange={event => self.send(ChangeInput(ReactEvent.Form.target(event)##value, FirstInput))}/>
        </label>
        <label>(ReasonReact.string("Second Pokemon"))
          <input type_="number" onChange={event => self.send(ChangeInput(ReactEvent.Form.target(event)##value, SecondInput))} />
        </label>
        <button type_="submit">(ReasonReact.string("I Choose You"))</button>
      </form>
  },
}
