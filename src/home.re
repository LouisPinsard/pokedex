type values = {
  first: int,
  second: int
};

type state = {
  firstInput: int,
  secondInput: int,
}

type input =
  | FirstInput
  | SecondInput;

type action =
  | Nothing
  | ChangeInput(int, input)
  | Submit;

let component = ReasonReact.reducerComponent("Home")

let make = _children => {
  ...component,
  initialState: () => {
    firstInput: 0,
    secondInput: 0,
  },
  reducer: (action, state: state) =>
    switch action {
    | Nothing => ReasonReact.Update(state)
    | ChangeInput(newValue, FirstInput) => ReasonReact.Update({...state, firstInput: newValue})
    | ChangeInput(newValue, SecondInput) => ReasonReact.Update({...state, secondInput: newValue})
    | Submit => ReasonReact.NoUpdate;
    },
  render: self =>
    <form onSubmit=((_event) => self.send(Submit))>
      <label>(ReasonReact.string("First Pokemon"))
        <input type_="number" onChange={event => self.send(ChangeInput(ReactEvent.Form.target(event)##value, FirstInput))}/>
      </label>
      <label>(ReasonReact.string("Second Pokemon"))
        <input type_="number" onChange={event => self.send(ChangeInput(ReactEvent.Form.target(event)##value, SecondInput))} />
      </label>
      <button type_="submit">(ReasonReact.string("I Choose You"))</button>
    </form>
}
