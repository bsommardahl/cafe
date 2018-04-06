open OrderData;

type state = {orders: list(Order.orderVm)};

type action =
  | SelectOrder(Order.orderVm)
  | OrdersLoaded(list(Order.orderVm));

let component = ReasonReact.reducerComponent("OpenOrders");

let loadOpenOrders = send =>
  OrderStore.getOpenOrders()
  |> Js.Promise.then_(orders => {
       let vms = orders |> List.map(OrderConversion.vmFromExistingOrder);
       Js.log("Dispatching orders: " ++ string_of_int(vms |> List.length));
       send(OrdersLoaded(vms));
       Js.Promise.resolve();
     })
  |> ignore;

let make = _children => {
  ...component,
  initialState: () => {orders: []},
  didMount: self => {
    loadOpenOrders(self.send);
    ReasonReact.NoUpdate;
  },
  subscriptions: self => [
    Sub(
      () => Js.Global.setInterval(() => loadOpenOrders(self.send), 5000),
      Js.Global.clearInterval,
    ),
  ],
  reducer: (action, _state) =>
    switch (action) {
    | OrdersLoaded(orders) =>
      ReasonReact.UpdateWithSideEffects(
        {orders: orders},
        (
          self =>
            Js.log(
              "Loading orders into state: "
              ++ string_of_int(self.state.orders |> List.length),
            )
        ),
      )
    | SelectOrder(order) =>
      ReasonReact.SideEffects(
        (
          _self =>
            switch (order.id) {
            | Some(id) => ReasonReact.Router.push("order?orderId=" ++ id)
            | None => ()
            }
        ),
      )
    },
  render: self => {
    let selectOpenOrder = (order: Order.orderVm) =>
      self.send(SelectOrder(order));
    <div className="open-orders">
      (
        self.state.orders
        |> List.map((o: OrderData.Order.orderVm) =>
             <OpenOrderCard order=o onSelect=(_event => selectOpenOrder(o)) />
           )
        |> Array.of_list
        |> ReasonReact.arrayToElement
      )
    </div>;
  },
};