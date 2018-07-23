let str = ReasonReact.stringToElement;

let component = ReasonReact.statelessComponent("EditDiscountModal");

let make =
    (
      ~onCancel=() => (),
      ~discount: option(Discount.t)=None,
      ~onSubmit,
      ~discounts: list(Discount.t),
      ~isOpen=false,
      ~label: string,
      _children,
    ) => {
  ...component,
  render: _self =>
    <div>
      <BsReactstrap.Modal isOpen className="modal">
        <BsReactstrap.ModalHeader className="modal-header">
          (ReactUtils.sloc(label))
          <Button
            className="exit-modal-button-card exit-card"
            onClick=(_ => onCancel())
            iconClass="fas fa-times"
            local=true
          />
        </BsReactstrap.ModalHeader>
        <BsReactstrap.ModalBody className="modal-content">
          <div>
            <DiscountEdit
              discount
              discounts
              onCancel=(_ => onCancel())
              onSubmit
            />
          </div>
        </BsReactstrap.ModalBody>
        <BsReactstrap.ModalFooter className="modal-footer" />
      </BsReactstrap.Modal>
    </div>,
};