type state = {
  language: string,
  deviceId: string,
};

type action =
  | LoadConfig
  | ConfigLoaded(Config.App.t)
  | SaveConfig
  | DeviceIdChanged(string)
  | LanguageChanged(string);

let component = ReasonReact.reducerComponent("SyncManagement");

let make = _children => {
  ...component,
  initialState: () => {language: "EN", deviceId: ""},
  didMount: self => {
    self.send(LoadConfig);
    ReasonReact.NoUpdate;
  },
  reducer: (action, state) =>
    switch (action) {
    | LoadConfig =>
      ReasonReact.SideEffects(
        (self => self.send(ConfigLoaded(Config.App.get()))),
      )
    | ConfigLoaded(config) =>
      ReasonReact.Update({
        language: config.language,
        deviceId: config.deviceId,
      })
    | SaveConfig =>
      ReasonReact.SideEffects(
        (
          _self =>
            Config.App.set({
              language: state.language,
              deviceId: state.deviceId,
            })
        ),
      )
    | LanguageChanged(newVal) =>
      ReasonReact.Update({...state, language: newVal})
    | DeviceIdChanged(newVal) =>
      ReasonReact.Update({...state, deviceId: newVal})
    },
  render: self => {
    let getVal = ev => ReactDOMRe.domElementToObj(
                         ReactEventRe.Form.target(ev),
                       )##value;
    <div className="config-management">
      <h2> (ReactUtils.s("Application")) </h2>
      <table>
        <tr>
          <th> (ReactUtils.s("Language")) </th>
          <td>
            <input
              value=self.state.language
              onChange=(ev => self.send(LanguageChanged(getVal(ev))))
            />
          </td>
        </tr>
        <tr>
          <th> (ReactUtils.s("DeviceId")) </th>
          <td>
            <input
              value=self.state.deviceId
              onChange=(ev => self.send(DeviceIdChanged(getVal(ev))))
            />
          </td>
        </tr>
      </table>
      <button className="card" onClick=((_) => self.send(SaveConfig))>
        (ReactUtils.s("Guardar"))
      </button>
    </div>;
  },
};