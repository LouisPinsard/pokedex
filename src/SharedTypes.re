type poketype =
  | Fire
  | Water;

type pokemon = {
  name: string,
  id: int,
  spriteUrl: string,
};

type action =
  | Click
  | LoadPokemonFailed
  | LoadedPokemon(pokemon);

type state =
  | Loading
  | NotAsked
  | Failure
  | Success(pokemon);
