pub fn try_solve(expression: &str) -> Result<f64, ()> {
    let mut stack = Vec::new();
    let mut expr = expression;

    while expr.len() != 0 {
        expr = ignore_whitespace(expr);
        let trynumber = parse_number(&expr);
        if trynumber.is_ok() {
            let (number, new_expr) = trynumber.expect("");
            stack.push(number);
            expr = new_expr;
        } else {
            let (operator, new_expr) = parse_operator(&expr)?;
            expr = new_expr;

            if stack.len() < 2 {
                return Err(());
            }

            evaluate(&mut stack, operator);
        }
    }

    if stack.len() != 1 {
        return Err(());
    }

    return Ok(stack[0]);
}

fn evaluate(stack: &mut Vec<f64>, operator: char) -> Option<()> {
    let opr2 = stack.pop()?;
    let opr1 = stack.pop()?;
    match operator {
        '+' => stack.push(opr1 + opr2),
        '-' => stack.push(opr1 - opr2),
        '*' => stack.push(opr1 * opr2),
        '/' => stack.push(opr1 / opr2),
        _ => return None
    }

    return Some(());
}

fn ignore_whitespace(expression: &str) -> &str {
    let bytes = expression.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item != b' ' {
            return &expression[i..];
        }
    }

    &expression[expression.len()..]
}

fn parse_operator(expression: &str) ->Result<(char, &str), ()> {
    let expr = ignore_whitespace(expression);

    return match expression.chars().nth(0) {
        None => Err(()),
        Some(c) => Ok((c, &expr[1..])),
    };

}

fn parse_number(expression: &str) -> Result<(f64, &str), ()> {
    let mut number: f64= 0.0;

    let first = match expression.chars().nth(0) {
        None => return Err(()),
        Some(c) => c,
    };

    if first < '0' || first > '9' {
        return Err(());
    }

    for (idx, chr) in expression.chars().into_iter().enumerate() {
        match chr {
            '0'..='9' => {
                match chr.to_digit(10) {
                    None => return Err(()),
                    Some(num) => {
                        number *= 10.0;
                        number += num as f64;
                    }
                }
            }

            _ => return Ok((number, &expression[idx..])),
        }
    }

    return Ok((number, &expression[expression.len()..]));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn zero() {
        let result = try_solve(&String::from("0"));
        assert_eq!(result.is_ok(), true);
        assert_eq!(result.unwrap(), 0.0);
    }

    #[test]
    fn operation() {
        assert_eq!(try_solve("1 2+").unwrap(), 3.0);
    }

    #[test]
    fn operator_only() {
        assert_eq!(try_solve("+").is_err(), true);
    }

    #[test]
    fn too_many_operators() {
        assert_eq!(try_solve("2 +").is_err(), true);
    }

    #[test]
    fn too_many_numbers() {
        assert_eq!(try_solve("1 2 3 +").is_err(), true);
    }

    #[test]
    fn basic_expression() {
        assert_eq!(try_solve(" 5 2 *").unwrap(), 10.0);
    }

    #[test]
    fn qexpression_nnnoo() {
        assert_eq!(try_solve("1 2 3 + -").unwrap(), -4.0);
    }

    #[test]
    fn expression_nnono() {
        assert_eq!(try_solve("1 2 +  3 -").unwrap(), 0.0);
    }

    #[test]
    fn expression_nnonnoo() {
        assert_eq!(try_solve("1 2 + 3 4 - -").unwrap(), 4.0);
    }

    #[test]
    fn expression_extreme() {
        assert_eq!(try_solve("1 1 1 1 1 1 1 1 1 1 1 * 1 1 1 1 1 1 1 1 1 / 1 1 * / * / / * * / / * / * * / * / * / *").unwrap(), 1.0);
    }
}
