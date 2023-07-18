import java.awt.*;
import java.util.ArrayList;

class Salesperson extends Entity {
    private Product type;
    private int maxStock;
    private int stock;
    private Strategy strategy;
    private Position targetPosition;
    private int lastMove;

    public enum Strategy {
        Random,
        MiddleGround,
    }

    public Salesperson(double x, double y, Product type, int maxStock, Strategy strategy) {
        super(x, y);
        this.type = type;
        this.maxStock = maxStock;
        this.stock = maxStock;
        this.strategy = strategy;
        targetPosition = null;
        lastMove = 0;
    }

    @Override
    public void draw(Graphics2D g2d) {
        String text = type.abbreviate() + "," + String.valueOf(stock) + ",";
        switch (strategy) {
            case Random:
                text += "R";
                break;
            case MiddleGround:
                text += "M";
                break;
        }
        drawHelper(g2d, text, Color.BLUE);
    }

    public Product getType() {
        return type;
    }

    @Override
    public void step() {
        lastMove += 1;
        if (lastMove == Common.getSalespersonMoveInterval()) {
            lastMove = 0;
            targetPosition = null;
        }

        if (stock == 0) {
            if (position.getIntX() < Common.getWindowWidth() - position.getIntX()) {
                position.setX(position.getX() - Common.getSalespersonMovementSpeed());
            } else {
                position.setX(position.getX() + Common.getSalespersonMovementSpeed());
            }
        } else if (targetPosition == null) {
            double x;
            double y;
            switch (strategy) {
                case Random:
                    x = Common.randInt(0, Common.getWindowWidth() - 2 * Common.getEntityDiameter());
                    y = Common.randInt(0, Common.getWindowHeight() - 2 * Common.getEntityDiameter());
                    targetPosition = new Position(x, y);
                    break;
                case MiddleGround:
                    x = 0;
                    y = 0;
                    for (Customer c : Common.getCustomerList()) {
                        x += c.getPosition().getX();
                        y += c.getPosition().getY();
                    }
                    x /= Common.getCustomerNo();
                    y /= Common.getCustomerNo();
                    targetPosition = new Position(x, y);
                    break;
            }
        } else {
            /* System.out.println("Salesperson moving towards target position");
            double dist = this.getPosition().distanceTo(targetPosition.getX(), targetPosition.getY());
            if (dist > 0.1) {
                double x = targetPosition.getX() - this.getPosition().getX();
                double y = targetPosition.getY() - this.getPosition().getY();
                if (dist > Common.getSalespersonMovementSpeed()) {
                    double vx = x / dist * Common.getSalespersonMovementSpeed();
                    double vy = y / dist * Common.getSalespersonMovementSpeed();
                    getPosition().setX(getPosition().getX() + vx);
                    getPosition().setY(getPosition().getY() + vy);
                } else {
                    System.out.println("Salesperson arrived target position");
                    getPosition().setX(x);
                    getPosition().setY(y);
                }
            } */
            double targetX = targetPosition.getX();
            double targetY = targetPosition.getY();
            double dist = this.getPosition().distanceTo(targetX, targetY);
            if (dist <= 2 * Common.getEntityDiameter()) {

            } else {
                double x = targetX - this.getPosition().getX();
                double y = targetY - this.getPosition().getY();
                if (dist > Common.getSalespersonMovementSpeed()) {
                    double vx = x / dist * Common.getSalespersonMovementSpeed();
                    double vy = y / dist * Common.getSalespersonMovementSpeed();
                    getPosition().setX(getPosition().getX() + vx);
                    getPosition().setY(getPosition().getY() + vy);
                } else {
                    getPosition().setX(x);
                    getPosition().setY(y);
                }
            }
        }
    }
}