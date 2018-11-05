open SharedTypes;

let component = ReasonReact.reducerComponent("PokemonList");

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

let make = _children => {
  ...component,
  initialState: () => NotAsked,
  reducer: (action: action, _state) =>
    switch action {
    | Click => ReasonReact.UpdateWithSideEffects(
        Loading,
        (
          self => Js.Promise.(
            fetchPokemon()
            |> then_(result =>
              switch result {
              | Some(pokemon) => resolve(self.send(LoadedPokemon(pokemon)))
              | None => resolve(self.send(LoadPokemonFailed))
              }
            )
            |> ignore
          )
        ),
      )
    | LoadedPokemon(pokemon) => ReasonReact.Update(Success(pokemon))
    | LoadPokemonFailed => ReasonReact.Update(Failure)
    },
  render:self =>
    switch (self.state) {
    | NotAsked =>
      <div>
        (ReasonReact.string("Click to start load pokemons"))
        <button onClick=(_event => self.send(Click))>
          (ReasonReact.string("Load pokemons"))
        </button>
      </div>
    | Loading => <div> (ReasonReact.string("Loading...")) </div>
    | Failure => <div> (ReasonReact.string("Something went wrong!")) </div>
    | Success(pokemon) =>
        <Pokemon pokemon= pokemon/>
    },
  }
