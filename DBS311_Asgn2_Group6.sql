--Group 6
--Name: Rujal Tuladhar
--seneca id: 154594188
--email : rtuladhar@myseneca.ca

--Name: Long Nguyen
--seneca id: 155176183
--email : lnguyen97@myseneca.ca



set serveroutput on;

--Add order
create or replace PROCEDURE ADD_ORDER(customer_id IN NUMBER, new_order_id OUT NUMBER)
IS
    sysdate DATE;
BEGIN
    SELECT MAX(order_id) + 1 INTO new_order_id FROM orders;
    SELECT SYSDATE INTO sysdate FROM DUAL;
    INSERT INTO orders
    VALUES (new_order_id, customer_id, 'Shipped', 56, sysdate);
    
    EXCEPTION
        WHEN OTHERS THEN
            DBMS_OUTPUT.PUT_LINE('SOME ERROR HAPPENED');    
END ADD_ORDER;

--Add order item
create or replace PROCEDURE ADD_ORDER_ITEM( orderId IN order_items.order_id%type,
                                            itemId IN order_items.item_id%type, 
                                            productId IN order_items.product_id%type, 
                                            quantity IN order_items.quantity%type,
                                            price IN order_items.unit_price%type)
IS
BEGIN
    INSERT INTO order_items
    VALUES(orderId,itemId,productId,quantity,price);
    EXCEPTION
        WHEN OTHERS THEN
            DBMS_OUTPUT.PUT_LINE('SOME ERROR HAPPENED');    
END ADD_ORDER_ITEM;

--find customer

create or replace PROCEDURE FIND_CUSTOMER(customer_id IN NUMBER, found OUT NUMBER)
IS
BEGIN
    SELECT COUNT(*) INTO found FROM CUSTOMERS WHERE customer_id=FIND_CUSTOMER.customer_id;
    EXCEPTION
    WHEN no_data_found THEN
    	found := 0;
END FIND_CUSTOMER;

--find product

create or replace PROCEDURE FIND_PRODUCT(product_id IN NUMBER, price OUT products.list_price%TYPE) 
IS
BEGIN
  SELECT list_price INTO price FROM products WHERE product_id=FIND_PRODUCT.product_id;
          DBMS_OUTPUT.PUT_LINE(price);
  EXCEPTION
    WHEN no_data_found THEN
    	price := 0;
END FIND_PRODUCT;