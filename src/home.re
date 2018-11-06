type action =
  | Click;

type values = {
  first: int,
  second: int
};

let component = ReasonReact.statelessComponent("Home")

let make = _children => {
  ...component,
  render: _self =>
  <form onSubmit=((values) => Js.log(values))>
    <label>(ReasonReact.string("First Pokemon"))
      <input type_="number"/>
    </label>
    <label>(ReasonReact.string("Second Pokemon"))
      <input type_="number" onChange=((t) => Js.log(t))  />
    </label>
    <button type_="submit">(ReasonReact.string("I Choose You"))</button>
  </form>
}
