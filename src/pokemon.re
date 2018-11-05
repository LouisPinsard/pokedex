open SharedTypes

let component = ReasonReact.statelessComponent("Pokemon")

let make = (~pokemon: pokemon, _children) => {
  ...component,
  render: _self =>
    <div>
      <h2> (ReasonReact.string(pokemon.name)) </h2>
      <img src=pokemon.spriteUrl />
    </div>
};

