open Util;

module RealCafeStore =
  CafeStore.Make(
    {
      let connect = PouchdbImpl.connect;
    },
  );

type viewing =
  | Tags
  | Products(string);

type state = {
  allProducts: list(Product.t),
  tags: list(string),
  viewing,
  order: OrderData.Order.orderVm,
};

type action =
  | SelectTag(string)
  | SelectProduct(Product.t)
  | DeselectTag
  | LoadOrder(OrderData.Order.orderVm)
  | CloseOrderScreen;

let buildOrderItem = (product: Product.t) : OrderData.Order.orderItem => {
  product,
  addedOn: Js.Date.now(),
  salePrice: product.suggestedPrice,
};

let buildNewOrder = (customerName: string) : OrderData.Order.orderVm => {
  id: None,
  customerName,
  orderItems: [],
  createdOn: Js.Date.now(),
  paidOn: None,
  amountPaid: None,
  paymentTakenBy: None,
  lastUpdated: None,
  removed: false,
};

let vmFromExistingOrder = (o: OrderData.Order.order) => {
  let vm: OrderData.Order.orderVm =
    OrderData.Order.{
      id: Some(o.id),
      customerName: o.customerName,
      orderItems: o.orderItems,
      createdOn: o.createdOn,
      paidOn: o.paidOn,
      amountPaid: o.amountPaid,
      paymentTakenBy: o.paymentTakenBy,
      lastUpdated: o.lastUpdated,
      removed: false,
    };
  vm;
};

let dbUrl = "http://localhost:5984/orders";

let component = ReasonReact.reducerComponent("Order");

let make = (~finishedWithOrder: OrderData.Order.orderVm => unit, _children) => {
  ...component,
  reducer: (action, state) =>
    switch (action) {
    | LoadOrder(order) => ReasonReact.Update({...state, order})
    | SelectTag(tag) =>
      ReasonReact.Update({...state, viewing: Products(tag)})
    | DeselectTag => ReasonReact.Update({...state, viewing: Tags})
    | CloseOrderScreen =>
      ReasonReact.SideEffects((_self => finishedWithOrder(state.order)))
    | SelectProduct(product) =>
      ReasonReact.Update({
        ...state,
        order: {
          ...state.order,
          orderItems:
            List.concat([
              state.order.orderItems,
              [buildOrderItem(product)],
            ]),
        },
      })
    },
  initialState: () => {
    let queryString = ReasonReact.Router.dangerouslyGetInitialUrl().search;
    let customerName =
      switch (Util.QueryParam.get("customerName", queryString)) {
      | Some(name) => name
      | None => "Cliente"
      };
    let products = Product.getProducts();
    {
      allProducts: products,
      tags: Product.getTags(products),
      viewing: Tags,
      order: buildNewOrder(customerName),
    };
  },
  didMount: ({reduce}) => {
    let dispatch = (order: OrderData.Order.orderVm) =>
      Js.Promise.resolve(reduce(() => LoadOrder(order)));
    let convertToVm = (order: OrderData.Order.order) =>
      Js.Promise.resolve(vmFromExistingOrder(order));
    let queryString = ReasonReact.Router.dangerouslyGetInitialUrl().search;
    switch (Util.QueryParam.get("orderId", queryString)) {
    | None => ReasonReact.NoUpdate
    | Some(orderId) =>
      /* Js.Promise.(
        RealCafeStore.get(orderId)
        |> then_(convertToVm)
        |> then_(dispatch)
        |> ignore
      ); */
      ReasonReact.NoUpdate;
    };
    /* the error in the compiler is here */
  },
  render: self => {
    let deselectTag = _event => self.send(DeselectTag);
    let selectTag = tag => self.send(SelectTag(tag));
    let selectProduct = product => self.send(SelectProduct(product));
    <div className="Order">
      <h1> (s("Order")) </h1>
      <h2> (s("Name:")) (s(self.state.order.customerName)) </h2>
      (
        switch (self.state.order.id) {
        | None => <div />
        | Some(id) => <h2> (s("Id:")) (s(id)) </h2>
        }
      )
      (
        switch (self.state.viewing) {
        | Tags =>
          self.state.tags
          |> List.map(tag => <div> <TagCard onSelect=selectTag tag /> </div>)
          |> Array.of_list
          |> ReasonReact.arrayToElement
        | Products(tag) =>
          <div>
            <button onClick=deselectTag> (s("Atras")) </button>
            (
              Product.filterProducts(tag, self.state.allProducts)
              |> List.map(product =>
                   <ProductCard onSelect=selectProduct product />
                 )
              |> Array.of_list
              |> ReasonReact.arrayToElement
            )
          </div>
        }
      )
      <OrderItems orderItems=self.state.order.orderItems />
      <OrderTotals orderItems=self.state.order.orderItems />
      <OrderActions
        order=self.state.order
        onFinish=((_) => self.send(CloseOrderScreen))
      />
    </div>;
  },
};