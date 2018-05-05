let component = ReasonReact.statelessComponent("Admin");

let make = _children => {
  ...component,
  render: _self => {
    let go = (u: string) : unit => ReasonReact.Router.push(u);
    <div className="admin-menu">
      <div className="header">
        <div className="header-options">
          (ReactUtils.s("Gestion del Programa"))
        </div>
      </div>
      <div className="admin-menu-buttons">
        <Button local=true onClick=((_) => go("logs")) label="admin.logs" />
        <Button
          local=true
          onClick=((_) => go("products"))
          label="admin.products"
        />
        <Button
          local=true
          onClick=((_) => go("expenseTypes"))
          label="admin.expenseTypes"
        />
        <Button
          local=true
          onClick=((_) => go("vendors"))
          label="admin.vendors"
        />
        <Button
          local=true
          onClick=((_) => go("config"))
          label="admin.config"
        />
      </div>
    </div>;
  },
};